#include <Include.hpp>
#include <classes/Class_Include.hpp>
#include <server/preperation/preperation.hpp>
#include <server/connection/connection.hpp>

//this is the main function, the program starts and ends here

auto main() -> int {

	printf("Starting Up Client..\n");
	//prepare connection data and run the "constructor"
	std::string SERVER_ADDRESS = "To get ur servers ip open the cmd and enter 'ipconfig', the ipv4 is what u need";
	std::string PORT = "9000";

	Client::client()->get_instance()->Init(SERVER_ADDRESS, PORT);
	//init our sockets
	if (!Client::client()->get_instance()->Initialize()) {
		log("Failed to Initialize Client.\n");
		return 1;
	}
	//connect to our server

    bool connected = false;//init this as false
    for (int attempt = 1; attempt <= 2 + 1; ++attempt) { // try to connect: if it connects we continue with our flow, if not we retry 3 times with a interval of 5 sec
        if (Client::client()->get_instance()->Connect()) {
            connected = true;
            break;
        }
        else {
            log("Failed to Connect to Server, Attempt -> %d\n", attempt);
            if (attempt <= 3) { //if we still have attempts left wait 5 second before attempting again
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }
    }
    if (!connected) {
        log("Terminating Connection Progress, Press Enter to Terminate..\n");
        std::getchar(); //wait for enter
        return 1;
    }

	//start our gui and thread it
	n_graphic::Startup();


	getchar();
	return 0;
}