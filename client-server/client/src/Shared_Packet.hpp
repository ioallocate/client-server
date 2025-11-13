#pragma once
#include <Include.hpp>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

//Helper function to get the TempPath for our Json Packet File

inline std::string getTempPath() {
	try {
		return fs::temp_directory_path().string(); //make use of the filesystem include and retrieve the temp path as a string
	}
	catch (const std::exception& e) {
		return ""; //if we catch a exception we return empty handed
	}
}

namespace n_shared_packet {



	inline std::string packetname = "Garbage"; // Garbage Initialized Packet name
	inline std::string status = "Awaiting.."; // Status Variable that is used for the gui
	inline std::string packetpath = getTempPath(); // Packet Path, we init it to the temp, because our file is always located there and it saves time

	/*
	
		s_packet
		this structs acts as a compact and small container for holding our
		user dumped data. The global instance allows for use of the same object
		across the project meaning no data gets lost
	
	*/

	typedef struct s_packet {
		std::wstring osversion;
		std::string osname;
		double ram;
		double storage;
		std::vector<std::string> installed_software; 
	}packet, * p_packet;
	extern packet g_packet;

	inline void clearPacket() { //this is a simple function that cleans the current global packet allowing easy reuse
		g_packet.installed_software.clear();
		g_packet.storage = NULL;
		g_packet.ram = NULL;
		g_packet.osname.clear();
		g_packet.osversion.clear();
	}

}