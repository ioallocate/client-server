#include "server.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

#ifdef min
#undef min
#endif

Server::Server(const std::string& port, size_t threadPoolSize)
    //constructor
    : m_port(port)
    , m_threadPoolSize(threadPoolSize)
    , m_listenSocket(INVALID_SOCKET)
    , m_running(false)
    , m_clientCounter(0)
{
}

Server::~Server() {
    //deconstructor
    Stop();
}

/*

    Initialize()
    This is the core function of making this server work.
    It handles the server setup and prepares it for Clients.

*/

bool Server::Initialize() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //Initializing the Sockets
    if (result != 0) {
        log("Failed to Start the Sockets, Result -> %d\n", result);
        return false;
    }
    //configuring the server variables
    struct addrinfo hints = {}, * addrResult = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //resolve only the port and initialize our configuration, and not the address since we are the server 
    result = getaddrinfo(NULL, m_port.c_str(), &hints, &addrResult); 
    if (result != 0) {
        log("getaddrinfo failed -> %d\n", result);
        WSACleanup();
        return false;
    }
    //create our server based socket, after resolving the addrResult
    m_listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (m_listenSocket == INVALID_SOCKET) {
        log("Failed to Create the Server Socket -> %d\n", WSAGetLastError());
        freeaddrinfo(addrResult);
        WSACleanup();
        return false;
    }
    //this configures our endpoint and binds the address to our socket
    result = bind(m_listenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    freeaddrinfo(addrResult);
    if (result == SOCKET_ERROR) {
        log("Bind Failed -> ", WSAGetLastError());
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    //after configuring our sockets we now listen for incoming requests from clients
    result = listen(m_listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        log("Failed to listen -> %d\n", WSAGetLastError());
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    log("Server Started on Port -> %s\n", this->m_port);
    return true;//if everything went right we return true
}

void Server::Start() {
    m_running = true;// set the running state to true and prepare our workerthread
    for (size_t i = 0; i < m_threadPoolSize; ++i) {
        m_workerThreads.emplace_back(&Server::WorkerThread, this);
    }
    AcceptLoop(); //give the green light for the client
}

void Server::Stop() {
    //this is a "stop" guard it ensure this only runs once
    if (!m_running.exchange(false)) {
        return;
    }

    log("Stopping Server..\n");

    //acquire a mutex and lock each client for cleanup
    {
        std::lock_guard<std::mutex> lock(m_clientStatesMutex);
        for (auto& pair : m_clientStates) {
            CleanupSocket(pair.second->socket);
        }
        m_clientStates.clear();
    }

    m_queueCV.notify_all(); //wake up all threads and syncronize them
    for (auto& thread : m_workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    if (m_listenSocket != INVALID_SOCKET) {
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }//if the socket is still valid, close it and cleanup our Sockets

    WSACleanup();
    log("Server Stopped\n");
}

void Server::AcceptLoop() {
    while (m_running) {//while the server is up listen for incoming requests
        SOCKET clientSocket = accept(m_listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if (m_running) {
                log("Failed to Accept Client -> %d\n", WSAGetLastError());
            }
            continue;
        }
        int clientId = ++m_clientCounter;
        log("Client Connected -> %d\n", clientId);

        auto clientState = std::make_shared<ClientState>(clientSocket, clientId);
        {
            std::lock_guard<std::mutex> lock(m_clientStatesMutex);
            m_clientStates[clientId] = clientState;
        }
        //Push the client back
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_jobQueue.push({ clientSocket, clientId });
        }
        m_queueCV.notify_one();
    }
}


void Server::WorkerThread() {
    while (m_running) {
        ClientJob job;
        {
            // lock and wait for a job or the server stops
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCV.wait(lock, [this] { return !m_jobQueue.empty() || !m_running; });

            // exit if the queue is empty or the server stops
            if (!m_running && m_jobQueue.empty()) {
                break;
            }

            //restart if the loop if the queue is empty
            if (m_jobQueue.empty()) {
                continue;
            }

            //Acquire the next avaible job from the queue
            job = m_jobQueue.front();
            m_jobQueue.pop();
        }

        //now process the client
        HandleClient(job.clientSocket, job.clientId);
    }
}



void Server::HandleClient(SOCKET clientSocket, int clientId) {
    std::shared_ptr<ClientState> clientState;
    {
        std::lock_guard<std::mutex> lock(m_clientStatesMutex);
        auto it = m_clientStates.find(clientId);
        if (it == m_clientStates.end()) {
            log("Invalid Client State for Client\n");
            CleanupSocket(clientSocket);
            return;
        }
        clientState = it->second;
    }

    // Read requests from this client in a loop
    while (m_running) {
        // Read filename length
        int filenameLength = 0;
        if (!ReceiveAll(clientSocket, (char*)&filenameLength, sizeof(filenameLength))) {
            log("Client Disconnected -> %d\n", clientId);
            CleanupClient(clientId);
            return;
        }

        if (filenameLength <= 0 || filenameLength > 260) {
            log("Invalid filename length from client\n");
            CleanupClient(clientId);
            return;
        }

        // Read filename
        std::vector<char> filenameBuffer(filenameLength + 1, 0);
        if (!ReceiveAll(clientSocket, filenameBuffer.data(), filenameLength)) {
            log("Failed to recieve filename from client\n");
            CleanupClient(clientId);
            return;
        }
        std::string filename(filenameBuffer.data());

        // Read file size
        long long fileSize = 0;
        if (!ReceiveAll(clientSocket, (char*)&fileSize, sizeof(fileSize))) {
            std::cerr << "Failed to receive file size from client " << clientId << std::endl;
            CleanupClient(clientId);
            return;
        }

        if (fileSize <= 0 || fileSize > 1073741824) { 
            std::cerr << "Invalid file size from client " << clientId << ": " << fileSize << std::endl;
            CleanupClient(clientId);
            return;
        }

        // Read file into memory
        std::vector<char> fileData(fileSize);
        if (!ReceiveAll(clientSocket, fileData.data(), (int)fileSize)) {
            std::cerr << "Failed to receive file data from client " << clientId << std::endl;
            CleanupClient(clientId);
            return;
        }

        log("Client %d request received: %s (%zu bytes) - queuing for processing\n",
            clientId, filename.c_str(), fileSize);

        // Queue request for processing
        {
            std::lock_guard<std::mutex> lock(clientState->queueMutex);
            clientState->requestQueue.push([this, clientState, filename, fileData, clientSocket]() {
                // Process the file
                std::string baseName = fs::path(filename).stem().string();
                std::string username = baseName;

                size_t underscorePos = baseName.find('_');
                if (underscorePos != std::string::npos) {
                    username = baseName.substr(0, underscorePos);
                }

                std::string dirPath = "clients\\" + username;
                std::string filePath = dirPath + "\\" + filename;//configure folder for current client

                try {
                    fs::create_directories(dirPath);
                }
                catch (const std::exception& e) {
                    log("Failed to create dir for current client\n");
                    return;
                }

                std::ofstream outFile(filePath, std::ios::binary);
                if (!outFile.is_open()) {
                    log("Failed to create file for current client\n");
                    return;
                }
                //write the recieved data and close the file
                outFile.write(fileData.data(), fileData.size());
                outFile.close();

                log("Data recieved and saved successfully!\n");

                // Send acknowledgment
                const char* ack = "OK";
                if (send(clientSocket, ack, 2, 0) == SOCKET_ERROR) {
                    log("Failed to Callback Client -> %d\n", clientState->clientId);
                }
                });
        }

        // Process the queue
        ProcessNextRequest(clientState);
    }
}


void Server::ProcessNextRequest(std::shared_ptr<ClientState> clientState) {
    bool expected = false;
    if (!clientState->isProcessing.compare_exchange_strong(expected, true)) {
        return; // Already processing
    }

    // Process queued requests
    while (m_running) {
        std::function<void()> request;
        {
            std::lock_guard<std::mutex> lock(clientState->queueMutex);
            if (clientState->requestQueue.empty()) {
                clientState->isProcessing = false;
                return;
            }
            request = std::move(clientState->requestQueue.front());
            clientState->requestQueue.pop();
        }

        if (request) {
            request();
        }
    }

    clientState->isProcessing = false;
}

/*
    
    ReceiveAll()
    This Mirrors the client sided function and is designed to
    recieve all the bytes that got sent

*/
bool Server::ReceiveAll(SOCKET socket, char* buffer, int length) {
    int totalReceived = 0;
    while (totalReceived < length) {//Loop till the recieved bytes matches the length
        int bytesReceived = recv(socket, buffer + totalReceived, length - totalReceived, 0);
        if (bytesReceived <= 0) {
            return false;
        }
        totalReceived += bytesReceived;
    }
    return true;
}

//this created the parent folder where all the clients folder will be stored
bool Server::CreateClientDirectory(int clientId) {
    try {
        std::string dirPath = "clients\\client_" + std::to_string(clientId);
        fs::create_directories(dirPath);
        return true;
    }
    catch (const std::exception& e) {
        //treat silently
        return false;
    }
}

//cleanup the sockets 
void Server::CleanupSocket(SOCKET socket) {
    if (socket != INVALID_SOCKET) {
        shutdown(socket, SD_BOTH);
        closesocket(socket);
    }
}

void Server::CleanupClient(int clientId) {
    std::shared_ptr<ClientState> clientState;
    {
        std::lock_guard<std::mutex> lock(m_clientStatesMutex);
        auto it = m_clientStates.find(clientId);
        if (it != m_clientStates.end()) {
            clientState = it->second;
            m_clientStates.erase(it);
        }//lock the clientstate and delete it
    }

    if (clientState) { //if its valid, we clean it
        CleanupSocket(clientState->socket);
        log("client state cleaned up for client -> %d\n", clientId);
    }
}