#pragma once

/*

	This is a Super Header, that includes every needed dependencies 
	Gui, json helpers, Data dumper..

*/

#include <Include.hpp>


namespace n_graphic {


	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // window handler
	int Startup(); // start the gui

}

namespace n_os {

	std::string get_os();//get the operating system
	std::wstring get_os_version();// get the version of it

}

namespace n_ram {

	double get_ram(); // return the current amount of installed ram (not avaible ram)

}

namespace n_storage {

	double get_free_storage(); //get the free storage on all drives combined 

}

namespace n_software {

	std::vector<std::string> get_software(); // get a list of a all installed software

}

namespace n_json_name {

	std::string construct();
	//std::string get_current_user();
	//std::time_t get_unix_timestamp(); these are both internal functions of this namespace which should only be public for debugging
}

/*

	this either creates or loads the ip.cfg file

*/

namespace n_cfg {

	bool load_or_create_ip_config(std::string& outIp);

}