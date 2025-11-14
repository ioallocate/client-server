#include <Include.hpp>
#include <classes/Class_Include.hpp>
#include <server/preperation/preperation.hpp>
#include <server/connection/connection.hpp>
#include <server/precatch/precatch.hpp>

//this is the main function, the program starts and ends here

auto main() -> int {

    
    //std::getchar();
	printf("Starting Up Client..\n");

    //Initializing Dependencies
    CreateClientServerFolders();
	//prepare connection data and run the "constructor"
    std::string SERVER_ADDRESS = "Garbage..";
    n_cfg::load_or_create_ip_config(SERVER_ADDRESS);
	 //To get your servers ipv4 open the cmd and enter 'ipconfig'
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
        std::cin; //wait for enter
        return 1;
    }
    Precatch* patcher = get_precatch_instance().get();
	log("Trying to Pre-Catch Dependencies..\n");
	log("Initializing Pre-Catch Module..\n");
    if (!patcher->Initialize(n_shared_packet::packetpath)) {
        log("Failed to Initialize Pre-Catch Module, Exiting..\n");
        return 1;
	}
	log("Standby..\n");
	//loop trough our custom roaming folder and put the full path of every found file into our queue
    patcher->catched_files(n_shared_packet::packetpath);
    if (patcher->get_file_queue().empty()) {
        log("No Dependencies Caught, Unlocking Manual Dump..\n");
        n_shared_packet::manual_dump = true;
        Sleep(1000);
    }
    else {
		log("Caught %d Dependencies, Sending to Server..\n", patcher->get_file_queue().size()); //send every file in our queue to the server
        for (const auto& file : patcher->get_file_queue()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(750)); 
			Client::client()->get_instance()->SendFile(file);//send the file
            std::error_code ec;
			if (std::filesystem::remove(file, ec)) { //delete the file locally after sending
				log("file %s Deleted Successfully Locally..\n", file.c_str());
            }
        }
        log("All Caught Dependencies Sent Successfully..\n");
		n_shared_packet::status = "All Caught Dependencies Sent Successfully.";
		n_shared_packet::manual_dump = true; //unlock manual dump
        Sleep(1500);
	}
	log("Starting GUI..\n");
	//start our gui and thread it
    std::thread(n_graphic::Startup).detach();

	log("Press anything to exit..\n");
	getchar();

	return 0;
}


