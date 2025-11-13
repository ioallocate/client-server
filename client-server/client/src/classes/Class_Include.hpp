#pragma once

/*

	This is a Super Header, that includes every needed dependencies 
	Gui, json helpers, Data dumper..

*/

#include <Include.hpp>
#include <Third Party/imgui/imgui_impl_win32.h>
#include <Third Party/imgui/imgui_impl_dx11.h>
#include <Third Party/imgui/imgui.h>

namespace n_graphic {

	extern ID3D11Device* g_pd3dDevice;
	extern ID3D11DeviceContext* g_pd3dDeviceContext;
	extern IDXGISwapChain* g_pSwapChain;
	extern bool                     g_SwapChainOccluded;
	extern UINT                     g_ResizeWidth, g_ResizeHeight;
	extern ID3D11RenderTargetView* g_mainRenderTargetView;
	bool CreateDeviceD3D(HWND hWnd); //configure directx11 device
	void CleanupDeviceD3D(); // clean up the device
	void CreateRenderTarget(); //prepare ui creation and buffers
	void CleanupRenderTarget(); //gui module cleanup
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