#include <server/connection/connection.hpp>
#include <Shared_Packet.hpp>

//this initializes the sockets

bool Client::Initialize() {
    WSADATA wsaData;
    int result = 0;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData); //startup the socket, this is a must
    if (result == 0) {
        log("Successfully Setup Socket\n");
        return true;
    }
    else {
        log("Failed to Setup Socket\n");
    }
    return false;//incase of an error we return false
}


void Client::acquireServerStatus(SOCKET& sock, const std::string& serverIP, int serverPort) {
    while (true) {
        // Check if socket is still valid
        char buffer;
        int result = recv(sock, &buffer, 1, MSG_PEEK);
        if (result == 0 || (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)) {
            log("Connection Lost, Attempting to Reconnect..\n");
            Client::client()->get_instance()->setconnected(false);
            bool reconnected = false;
            for (int attempt = 1; attempt <= 3; ++attempt) { // attempt to reconnect 3 times
                log("Reconnecting, Current Attempt -> %d\n", attempt);

                SOCKET newSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create a new socket to reconnect with
                if (newSock == INVALID_SOCKET) {
                    log("Failed to create a new socket\n");
                    break;
                }

                sockaddr_in serverAddr{}; //configure variables
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(serverPort);
                inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

                if (connect(newSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) { //attempt to reconnect
                    log("Reconnected with Server!\n");
                    Client::client()->get_instance()->setconnected(true);
                    closesocket(sock);
                    sock = newSock;
                    reconnected = true;
                    break;
                }
                else {
                    log("Reconnect Failed, Attempting to Reconnect in 5 Seconds.\n");
                    closesocket(newSock);//cleanup the leftovers
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }

            if (!reconnected) {
                log("Could not Reconnect after 3 Attempts.\n");
                log("Terminating Connection Progress, Press Enter to Terminate..\n");
                std::getchar();
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(2)); // Check every 2 seconds
    }
}


bool Client::Connect() {
    struct addrinfo hints = {}, * addrResult = nullptr; //declare and configure the variables
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int result = getaddrinfo(m_serverAddress.c_str(), m_port.c_str(), &hints, &addrResult); //Resolve Port and Address, puts the result into addrResult
    if (result != 0) {
        log("getaddrinfo failed -> %d\n", result);
        return false;
    }
    m_socket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); //create a socket based on the config, of the var addrResult
    if (m_socket == INVALID_SOCKET) {
        log("Socket failed -> %d\n", WSAGetLastError());
        freeaddrinfo(addrResult);
        return false;
    }
    result = connect(m_socket, addrResult->ai_addr, (int)addrResult->ai_addrlen); //Now it tries to connect using our setup socket, and configurations
    freeaddrinfo(addrResult);
    if (result == SOCKET_ERROR) {
        log("Connect failed -> %d\n", WSAGetLastError()); //get the last error and cleanup
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return false;
    }
    //
    this->m_is_connected = true;
    std::thread connectionMonitor(Client::acquireServerStatus, std::ref(m_socket), m_serverAddress,std::stoi(m_port));
    connectionMonitor.detach();

    


    log("Successfully Connected to Server\n"); //log true
    return true;
}

bool Client::LocalCleanup(std::string& path) {
    std::error_code ec;
    //this is a local cleanup to ensure the client is cleaned and prepared for the next request
    if (std::filesystem::remove(path, ec)) {
        log("Successfully Deleted Constructed File Locally.\n");
        n_shared_packet::packetname.clear();
        n_shared_packet::clearPacket();
        path.clear();
        path = get_unique_path();
        log("Buffers Cleared.\n");
        return true;
    }
    else {
        log("Failed to delete Packet File, logging for User debugging.\n");
        log("Error: %s\n", ec.message().c_str());
        log("Packet Name: %s\n", n_shared_packet::packetname);
        log("Packet Path: %s\n", n_shared_packet::packetpath);
        n_shared_packet::packetname.clear();
        path.clear();
        n_shared_packet::clearPacket();
        log("Buffers Cleared.\n");
        return false;
    }
}

bool Client::SendFile(const std::string& filePath) {
    //mutex 
    std::lock_guard<std::mutex> lock(m_socketMutex);
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        n_shared_packet::status = "Failed to Open File Packet...";
        log("Failed to Open File Packet.\n");
        return false;
    }

    long long fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize <= 0) {
        n_shared_packet::status = "Invalid File size..";
        log("Invalid File Size.\n");
        return false;
    }

    size_t pos = filePath.find_last_of("\\/");
    std::string filename = (pos != std::string::npos) ? filePath.substr(pos + 1) : filePath;

    log("Starting sending process..\n");

    // Send filename length
    int filenameLength = (int)filename.length();
    if (!SendAll((char*)&filenameLength, sizeof(filenameLength))) {
        n_shared_packet::status = "Failed to send Filename length..";
        log("Failed to send Filename length.\n");
        return false;
    }

    // Send filename
    if (!SendAll(filename.c_str(), filenameLength)) {
        n_shared_packet::status = "Failed to send Filename..";
        log("Failed to send Filename.\n");
        return false;
    }

    // Send file size
    if (!SendAll((char*)&fileSize, sizeof(fileSize))) {
        n_shared_packet::status = "Failed to send Filesize..";
        log("Failed to send Filesize\n");
        return false;
    }

    // Send file data in chunks
    const int BUFFER_SIZE = 8192;
    std::vector<char> buffer(BUFFER_SIZE);
    long long bytesSent = 0;

    while (bytesSent < fileSize) {
        int chunkSize = (int)std::min((long long)BUFFER_SIZE, fileSize - bytesSent);
        file.read(buffer.data(), chunkSize);
        //Loop trough the file till the size of the sent bytes is equals to the filesize
        if (!SendAll(buffer.data(), chunkSize)) {
            n_shared_packet::status = "Failed to sent File Data..";
            log("Failed to sent File Data\n");
            return false;
        }

        bytesSent += chunkSize;
    }

    file.close();
    log("File sent!\n");

    // Wait for server acknowledgment
    char ack[3] = {};
    int recvResult = recv(m_socket, ack, 2, 0);

    if (recvResult <= 0) {
        log("Server failed to Respond (recv returned %d, error: %d)\n", recvResult, WSAGetLastError());
        n_shared_packet::status = "Server Failed to Respond..";
        return false;
    }

    log("Server response -> %s\n", ack);

    if (!_strcmpi(ack, "OK")) {
        log("Server Response Received, STATUS OK\n");
        n_shared_packet::status = ack;
        LocalCleanup(n_shared_packet::packetpath);
    }
    else {
        log("Server sent unexpected response: %s\n", ack);
        n_shared_packet::status = "Server sent unexpected response";
        return false;
    }

    // the Lock is automatically released here
    return true;
}