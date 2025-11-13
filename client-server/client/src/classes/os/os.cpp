#include <classes/Class_Include.hpp>


namespace n_os {


    std::string get_os() {
        //check for os by defs
#if defined(_WIN32)
        return "Windows";
#else
        return "Unsupported OS";

#endif
    }


    std::wstring get_os_version() {
#ifndef _WIN32
        return L"Unsupported OS";
#else
        typedef struct _RTL_OSVERSIONINFOW {
            ULONG dwOSVersionInfoSize;
            ULONG dwMajorVersion;
            ULONG dwMinorVersion;
            ULONG dwBuildNumber;
            WCHAR szCSDVersion[128];
        } RTL_OSVERSIONINFOW;//needed definitions
        typedef LONG(WINAPI* RtlGetVersionPtr)(RTL_OSVERSIONINFOW*);
        RTL_OSVERSIONINFOW ver = {};
        ver.dwOSVersionInfoSize = sizeof(ver);
        HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
        if (hMod) {
            auto func = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion"); //get version trough a usermode shared dll
            if (func && func(&ver) == 0) {
                wchar_t buf[64];
                swprintf(buf, 64, L"%u.%u.%u",//construct the string
                    ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber);
                return std::wstring(buf);
            }
        }
        return L"Unsupported";
#endif
    }



}