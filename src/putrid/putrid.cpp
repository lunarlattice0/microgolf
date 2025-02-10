#include "putrid/putrid.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <raylib.h>
#include <fstream>
#include <sstream>

const std::unordered_map<std::string, std::string> MicrogolfFilePaths = {
    {"config", std::string(GetApplicationDirectory()) + "config.cfg"},
    {"menubg", std::string(GetApplicationDirectory()) + "./assets/out/mainmenu.png"},
};

Config LoadConfigFromFile(const char *filepath) {
    Config config;
    if (std::filesystem::is_empty(filepath)) {
        return config;
    }

    std::ifstream configFile(filepath);
    {
        cereal::JSONInputArchive iarchive(configFile);
        iarchive(config);
    }
    configFile.close();


    return config;
}

void SaveConfigToFile(const char *filepath, Config config) {
    std::ofstream configFile(filepath);
    {
        cereal::JSONOutputArchive oarchive(configFile);
        oarchive(config);
    }
    configFile.close();
}

// Validate
void CheckFilePaths() {
    for (auto it : MicrogolfFilePaths) {
        if (!std::filesystem::exists(it.second)) {
            TraceLog(LOG_ERROR, "Missing file: %s", it.second.c_str());
            // TODO: Figure out how to yell at people on macos and windows
            std::exit(1);
        }
    }
}
