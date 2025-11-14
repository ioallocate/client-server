#include <server/preperation/Formatting/json_format.hpp>
#include <Shared_Packet.hpp>

namespace fs = std::filesystem;

namespace n_json_formatting {

    /*
        packettoJson()
        this is a function meant to be used in another function
        it creates a object which will defines the packet content
        and gets assigned the data of our struct, at the end we return 
        our object, so other functions get a ready to use object
    
    */

    json packetToJson(const n_shared_packet::packet& p) {
        json j;
        j["osversion"] = wstring_to_utf8(p.osversion);
        j["osname"] = ansi_to_utf8(p.osname);
        std::vector<std::string> utf8_software;
        for (const auto& s : p.installed_software) {
            utf8_software.push_back(ansi_to_utf8(s));
        }
        j["installed_software"] = utf8_software;
        j["ram"] = p.ram;
        j["storage"] = p.storage;
        return j;
    }

    /*
    
          savePackettoJsonFile()
          here we use our packetToJson function to instantly prepare 
          the newly created object.
          tempPath acquires the path of the temp folder where the packet file is located
          we set the precision of the decimals and fix it, then dump it in pretty print
    
    */

    void savePacketToJsonFile(const std::string& filename) {
        try {
            const auto& packet = n_shared_packet::g_packet;
            json j = packetToJson(packet);

            fs::path tempPath = fs::path(get_unique_path()) / filename;

            std::ofstream file(tempPath);
            if (!file) throw std::runtime_error("Failed to open file in temp directory");

            file << std::fixed << std::setprecision(2) << j.dump(4);
            file.close();
            log("jason saved\n");
        }
        catch (const std::exception& e) {
            //treat errors silently
        }
    }


}