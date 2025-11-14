#include <server/precatch/precatch.hpp>

bool Precatch::Initialize(std::string& temppath) {
    if (!fs::exists(temppath) || !fs::is_directory(temppath)) {
        return false;
    }
    m_temppath = temppath;
    m_file_queue.clear();
    return true;
}


void Precatch::catched_files(const std::string& path) {
    try {
        // Check if path exists and is a directory
        if (!fs::exists(path)) {
            return;
        }

        // Recursively iterate through directory
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            // Only add files, not directories
            if (entry.is_regular_file()) {
				log("Pre-Caught File: %s\n", entry.path().string().c_str());
                this->m_file_queue.push_back(entry.path().string());
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        // Handle permission errors or other filesystem issues
        // Could log error or throw if needed
    }
}

std::unique_ptr<Precatch>& get_precatch_instance() {
    static std::unique_ptr<Precatch> instance = std::make_unique<Precatch>();
    return instance;
}

