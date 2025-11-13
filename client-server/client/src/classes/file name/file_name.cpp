#include <classes/Class_Include.hpp>

namespace n_json_name {

    //internal function

    std::time_t get_unix_timestamp() {
        auto now = std::chrono::system_clock::now(); //get the current clock
        return std::chrono::system_clock::to_time_t(now); //and convert it to unix
    }

    //internal function

    std::string get_current_user() {
        //prepare buffers and fix the sized
        char buffer[256];
        DWORD size = sizeof(buffer);
        if (!GetUserNameA(buffer, &size)) { //use windows api to get the current username
            throw std::runtime_error("Failed to get current user");
        }
        return std::string(buffer);
    }

    //public function

    std::string construct() {
        return get_current_user() + '_' + std::to_string(get_unix_timestamp()); //construct our filename using our 2 internal functions
    }

}