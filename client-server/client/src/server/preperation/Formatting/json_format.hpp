#include <Include.hpp>
#include <Third Party/json/json.hpp>
#include <Shared_Packet.hpp>

using json = nlohmann::json;

/*

    wstring_to_utf8()
    ansi_to_utf8()

    These are both helper function to help with the json parsing
    since json expects utf8

*/

inline std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size_needed = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        wstr.c_str(),
        static_cast<int>(wstr.size()),
        nullptr, 0,
        nullptr, nullptr
    );
    if (size_needed == 0)
        throw std::runtime_error("WideCharToMultiByte failed (calculate size)");
    std::string result(size_needed, 0);
    int converted = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        wstr.c_str(),
        static_cast<int>(wstr.size()),
        result.data(),
        size_needed,
        nullptr, nullptr
    );
    if (converted == 0)
        throw std::runtime_error("WideCharToMultiByte failed (conversion)");
    return result;
}

inline std::string ansi_to_utf8(const std::string& str) {
    if (str.empty()) return {};
    int wsize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), nullptr, 0);
    if (wsize == 0) throw std::runtime_error("MultiByteToWideChar failed");
    std::wstring wstr(wsize, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &wstr[0], wsize);
    int utf8_size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    if (utf8_size == 0) throw std::runtime_error("WideCharToMultiByte failed");
    std::string utf8_str(utf8_size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &utf8_str[0], utf8_size, nullptr, nullptr);
    return utf8_str;
}

namespace n_json_formatting {

    json packetToJson(const n_shared_packet::packet& p);
    void savePacketToJsonFile(const std::string& filename);
}