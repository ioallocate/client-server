#include <classes/Class_Include.hpp>

namespace n_software {

    std::vector<std::string> get_software()
    {
        std::vector<std::string> softwareList;
        struct RegLocation {
            HKEY root;
            const char* path;
        };//setup array and structures for more compact searcg

        RegLocation locations[] = {
            { HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall" },
            { HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall" },
            { HKEY_CURRENT_USER,  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall" }
        };// location of installed software
        for (const auto& loc : locations)
        {
            HKEY hKey;
            if (RegOpenKeyExA(loc.root, loc.path, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
            { // open the locations of the registry keys
                DWORD index = 0;
                char subKeyName[256];
                DWORD subKeyLen = sizeof(subKeyName);
                while (RegEnumKeyExA(hKey, index, subKeyName, &subKeyLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
                {
                    HKEY hSubKey;
                    if (RegOpenKeyExA(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
                    { 
                        char displayName[512];
                        DWORD size = sizeof(displayName);
                        DWORD type = 0;

                        if (RegQueryValueExA(hSubKey, "DisplayName", nullptr, &type, reinterpret_cast<LPBYTE>(displayName), &size) == ERROR_SUCCESS)
                        {
                            if (type == REG_SZ && strlen(displayName) > 0)
                            {
                                softwareList.emplace_back(displayName); // get the display name and add it to the array
                            }
                        }
                        RegCloseKey(hSubKey);
                    }

                    subKeyLen = sizeof(subKeyName);
                    ++index;
                }

                RegCloseKey(hKey); //close and cleanup
            }
        }
        return softwareList;
    }

}