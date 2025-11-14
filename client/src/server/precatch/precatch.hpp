#pragma once
#include <Include.hpp>
#include <Shared_Packet.hpp>

class Precatch {
public:

	bool Initialize(std::string& temppath);
	void catched_files(const std::string& path);
	std::vector<std::string>& get_file_queue() {
		return m_file_queue;
	}


private:

	std::vector<std::string> m_file_queue;
	std::string m_temppath;
};
extern std::unique_ptr<Precatch>& get_precatch_instance();