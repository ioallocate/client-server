#pragma once
#include <Include.hpp>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

//Helper function to get the TempPath for our Json Packet File


inline std::string get_unique_path() {
    wchar_t* appDataPath = nullptr;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPath);

    if (FAILED(result)) {
        return ""; // Return empty string on failure
    }

    // Convert wide string to regular string
    int size = WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, nullptr, 0, nullptr, nullptr);
    std::string appDataStr(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, &appDataStr[0], size, nullptr, nullptr);

    // Free the memory allocated by SHGetKnownFolderPath
    CoTaskMemFree(appDataPath);

    // Build the full path to the temp folder
    std::string tempPath = appDataStr + "\\client-server\\temp\\";
	std::cout << "Temp Path: " << tempPath << std::endl;
    return tempPath;
}


inline bool CreateClientServerFolders() {
    std::string path = get_unique_path();

    if (path.empty()) {
        return false;
    }

    // Extract client-server path
    size_t pos = path.find("\\temp");
    std::string clientServerPath = path.substr(0, pos);

    // Create "client-server" folder if it doesn't exist
    if (!CreateDirectoryA(clientServerPath.c_str(), nullptr)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            return false;
        }
    }

    // Create "temp" folder if it doesn't exist
    if (!CreateDirectoryA(path.c_str(), nullptr)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            return false;
        }
    }

    return true;
}

namespace n_shared_packet {

    
    inline std::atomic<bool> manual_dump = true;
	inline std::string packetname = "Garbage"; // Garbage Initialized Packet name
	inline std::string status = "Awaiting.."; // Status Variable that is used for the gui
    inline std::string packetpath = get_unique_path(); // Packet Path, we init it to the temp, because our file is always located there and it saves time

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