#include <classes/Class_Include.hpp>
#include <regex>

namespace n_cfg {

    static void trim_inplace(std::string& s) {
        const char* whitespace = " \t\n\r";
        auto start = s.find_first_not_of(whitespace);
        if (start == std::string::npos) { s.clear(); return; }
        auto end = s.find_last_not_of(whitespace);
        s = s.substr(start, end - start + 1);
    }

    bool load_or_create_ip_config(std::string& outIp) {
        char exePathBuf[MAX_PATH] = { 0 };
        if (GetModuleFileNameA(NULL, exePathBuf, MAX_PATH) == 0) {
            log("Could not get own PE Path\n");
            return false;
        }

        std::string exePath(exePathBuf);
        auto pos = exePath.find_last_of("\\/");
        std::string dir = (pos == std::string::npos) ? "." : exePath.substr(0, pos);
        std::string cfgPath = dir + "\\ip.cfg";

		//if the file exist read it
        {
            std::ifstream ifs(cfgPath);
            if (ifs) {
                std::string content;
                std::getline(ifs, content);
                trim_inplace(content);
                outIp = content;
                log("ip.cfg found: %s\n", outIp.c_str());
                return true;
            }
        }

		//if the file doesnt exist ask the user for input
        std::regex ipv4(R"(^(?:25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)(?:\.(?:25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)){3}$)");
        for (;;) {
            log("ip.cfg not found. Please Input your Servers Ipv4 Manually (will be updated in the File) : ");
            std::string input;
            if (!std::getline(std::cin, input)) {
                log("Error while reading Input.\n");
                return false;
            }
            trim_inplace(input);

            if (std::regex_match(input, ipv4)) {
                std::ofstream ofs(cfgPath);
                if (!ofs) {
                    log("Fehler: could not create File : '%s' .\n", cfgPath.c_str());
                    return false;
                }
                ofs << input;
                ofs.close();

                outIp = input;
                log("ip.cfg created with content : %s\n", outIp.c_str());
                return true;
            }
            else {
                log("Invalid Ipv4 Signature, please re-enter.\n");
            }
        }

        // Unreachable
        return false;
    }

} // namespace n_cfg