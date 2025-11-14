#pragma once

#define _CRT_SECURE_NO_WARNINGS
/*

    This is a Public header meant to be included wherever
    it contains every needed include for this project, macro definitions 
    and the log function, which can be switched off by undefining #DEBUG

*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif



#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <tchar.h>
#include <thread>
#include <cctype>
#include <limits>
#include <algorithm>
#include <mutex>
#include <fstream>
#include <locale> 
#include <codecvt>
#include <chrono>
#include <stdexcept>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <shellscalingapi.h> 
#include <shlobj.h>

//Needed Library Linkage
 
#pragma comment(lib, "Shcore.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shell32")
#pragma comment (lib, "Ole32.lib")


//undefine this to disable debug
#define DEBUG

 __forceinline void log(const char* format, ...)
{
#ifdef DEBUG
    constexpr size_t BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);
    std::cout << "[CLIENT->DEBUG] " << buffer << std::endl;
#endif
}

