#include <server/preperation/preperation.hpp>
#include <server/preperation/Formatting/json_format.hpp>
#include <classes/Class_Include.hpp>


namespace n_server_preperation {



    void ValidateDump(n_shared_packet::packet& pkt)
    {
        if (pkt.osname.empty())//validate os name by size checking
        {
            log("Operation System not found returning NULL..\n");
            n_shared_packet::status = "Missing Dependencies";
            pkt.osname.clear();
        }
        else if (pkt.osname.size() < 2 || pkt.osname.size() > 100)
        {
            log("OS Name is Invalid, Garbage not Filtered..\n");
            n_shared_packet::status = "Missing Dependencies";
            pkt.osname.clear();
        }
        if (pkt.osversion.empty())
        {
            log("OS Name is Invalid, Garbage not Filtered..\n");
            n_shared_packet::status = "Missing Dependencies";
            pkt.osversion.clear();
        }
        if (pkt.ram == 0.0) 
        {
            log("Invalid RAM, returning NULL..\n");
            n_shared_packet::status = "Missing Dependencies";
            pkt.ram = NULL;
        }
        if (pkt.storage < 0.0 || pkt.storage > 1'000'000.0)
        {
            log("Invalid Storage Amount, returning NULL..\n");
            n_shared_packet::status = "Missing Dependencies";
            pkt.ram = NULL;
        }
        pkt.installed_software.erase(
            std::remove_if(pkt.installed_software.begin(),
                pkt.installed_software.end(),
                [](const std::string& s) {
                    return s.empty() || s.size() > 256;
                }),
            pkt.installed_software.end()
        );//if once value is invalid we will still send the packet but just return NULL
     
    }

	int dumpdata()
	{
        //we are assigning our dump functions to our global structure to make it easier to retrieve input later on
        n_shared_packet::g_packet.osname = n_os::get_os();
        n_shared_packet::g_packet.osversion = n_os::get_os_version();
        n_shared_packet::g_packet.ram = n_ram::get_ram();
        n_shared_packet::g_packet.storage = n_storage::get_free_storage();
        n_shared_packet::g_packet.installed_software = n_software::get_software();
        
        //here we check our packet to see if there are missing values or garbage
        ValidateDump(n_shared_packet::g_packet);
        log("OS: %s %ls\n", n_shared_packet::g_packet.osname.c_str(), n_shared_packet::g_packet.osversion.c_str());
        log("RAM: %.2f GB\n", n_shared_packet::g_packet.ram);
        log("Storage: %.2f GB\n", n_shared_packet::g_packet.storage);
        log("Installed Software Count: %zu\n", n_shared_packet::g_packet.installed_software.size());
        //debug print the values
        n_shared_packet::packetname = n_json_name::construct() + ".json";
		n_shared_packet::packetpath.clear();
		n_shared_packet::packetpath = get_unique_path();
        n_shared_packet::packetpath += n_shared_packet::packetname;
        std::cout << n_shared_packet::packetpath << std::endl;
        n_json_formatting::savePacketToJsonFile(n_shared_packet::packetname);
        n_shared_packet::status = "Successfully Dumped Data";
        return 1;
	}

}