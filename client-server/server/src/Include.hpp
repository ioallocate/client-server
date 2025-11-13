#pragma once


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
#include <d3d11.h>
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
#include <atomic>
#include <queue>
#include <memory>
#include <condition_variable>
#include <functional>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "d3d11.lib")

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
    std::cout << "[SERVER->DEBUG] " << buffer << std::endl;
#endif
}

