#pragma once
#include <Include.hpp>
#include <Shared_Packet.hpp>


namespace n_server_preperation {

	int dumpdata(); // collect all the telemetry and put it into the global struct
	void ValidateDump(n_shared_packet::packet& pkt);
}