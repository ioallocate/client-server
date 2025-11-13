#include <Include.hpp>
#include <server/server.hpp>
#include <csignal>

//global server decl
Server* g_server = nullptr;

//handles shutdown safely
void SignalHandler(int signal) {
    if (g_server) {

        g_server->Stop();
    }
}

//this is the main function, the program starts and ends here

int main() {
    
    //Setup our "shutdown" Handlers
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler); 

    const std::string PORT = "9000"; //configure our port
    const size_t THREAD_POOL_SIZE = 20; 

    //construct a instance and Initialize it
    Server server(PORT, THREAD_POOL_SIZE);
    g_server = &server;

    if (!server.Initialize()) {
        log("Failed to Init the Server.\n");
        return 1;
    }
    //startup the server
    log("Initialization Done, Starting Server.\n");
    server.Start();

    return 0;
}