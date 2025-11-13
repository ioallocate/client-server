#include <classes/Class_Include.hpp>


namespace n_ram {

	double get_ram() {
        MEMORYSTATUSEX status;// set definitions and fix the size
        status.dwLength = sizeof(status);
        if (GlobalMemoryStatusEx(&status))//retrieve info
        {
            return static_cast<double>(status.ullTotalPhys) / 1'000'000'000.0; // cast it into a readable form (gigabyte, decimals)
        }
        return -1.0;
	}

}