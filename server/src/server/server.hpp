#pragma once
#include <Include.hpp>

//Public class for the Server Engine

class Server {
public:
    Server(const std::string& port, size_t threadPoolSize = 10);
    ~Server();
    bool Initialize();
    void Start();
    void Stop();

private:
    struct ClientJob {
        SOCKET clientSocket;
        int clientId;
    };

    // Per-client state management
    struct ClientState {
        SOCKET socket;
        int clientId;
        std::queue<std::function<void()>> requestQueue;
        std::mutex queueMutex;
        std::atomic<bool> isProcessing;

        ClientState(SOCKET s, int id)
            : socket(s), clientId(id), isProcessing(false) {
        }
    };

    std::string m_port;
    size_t m_threadPoolSize;
    SOCKET m_listenSocket;
    std::atomic<bool> m_running;
    std::atomic<int> m_clientCounter;

    std::vector<std::thread> m_workerThreads;
    std::queue<ClientJob> m_jobQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCV;

    // Per-client queue management
    std::map<int, std::shared_ptr<ClientState>> m_clientStates;
    std::mutex m_clientStatesMutex;

    void WorkerThread();
    void HandleClient(SOCKET clientSocket, int clientId);
    void ProcessNextRequest(std::shared_ptr<ClientState> clientState);
    bool ReceiveAll(SOCKET socket, char* buffer, int length);
    bool CreateClientDirectory(int clientId);
    void CleanupSocket(SOCKET socket);
    void CleanupClient(int clientId);
    void AcceptLoop();
};