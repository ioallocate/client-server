#pragma once
#include <Include.hpp>


//Public class for the client connection

class Client {
public:
    static Client* client(const std::string& serverAddress = "", const std::string& port = "") {
        static Client instance(serverAddress, port);
        return &instance;
    }
    void Init(const std::string& serverAddress, const std::string& port) {
        m_serverAddress = serverAddress;
        m_port = port;
    }

    Client* get_instance() {
        return this;
    }

    void setconnected(bool status) {
        this->m_is_connected = status;
    }

    bool is_connected() {
        return this->m_is_connected;
    }
    //core functions
    bool Initialize();
    bool Connect();
    bool SendFile(const std::string& filePath);
    bool LocalCleanup(std::string& path);
    static void acquireServerStatus(SOCKET& sock, const std::string& serverIP, int serverPort);
    //socket cleanup
    void Cleanup() {
        std::lock_guard<std::mutex> lock(m_socketMutex);
        if (m_socket != INVALID_SOCKET) {
            shutdown(m_socket, SD_BOTH);
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
        WSACleanup();
    }

private:
    Client(const std::string& serverAddress, const std::string& port)
        : m_serverAddress(serverAddress), m_port(port), m_socket(INVALID_SOCKET) {
    }

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    bool m_is_connected;
    std::string m_serverAddress;
    std::string m_port;
    SOCKET m_socket;
    std::mutex m_socketMutex;  // CRITICAL: Protects socket operations

    bool SendAll(const char* buffer, int length) {
        int totalSent = 0;
        while (totalSent < length) {
            int bytesSent = send(m_socket, buffer + totalSent, length - totalSent, 0);
            if (bytesSent == SOCKET_ERROR) {
                return false;
            }
            totalSent += bytesSent;
        }
        return true;
    }
};