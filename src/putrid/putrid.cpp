#include "putrid/putrid.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <raylib.h>
#include <fstream>
#include <filesystem>

AssetManager::AssetManager() {
    for (auto it : MicrogolfFilePaths) {
        if (!std::filesystem::exists(it.second)) {
            TraceLog(LOG_ERROR, "Missing file: %s", it.second.c_str());
           // TODO: Figure out how to yell at people on macos and windows
            std::exit(1);
        }
    }
}

Config AssetManager::LoadConfig() {
    Config config;
    if (std::filesystem::is_empty(MicrogolfFilePaths.at("config"))) {
        return config;
    }

    std::ifstream configFile(MicrogolfFilePaths.at("config"));
    {
        // New error recover strategy:
        // Attempt to deserialize
        // On failure, check which struct element is unfilled, and fill it with default
        // Save to file
        // Attempt to reload
        // repeat until no errors


        cereal::JSONInputArchive iarchive(configFile);
        iarchive(config);
    }
    configFile.close();

    return config;
}

void AssetManager::SaveConfig(Config config) {
    std::ofstream configFile(MicrogolfFilePaths.at("config"));
    {
        cereal::JSONOutputArchive oarchive(configFile);
        oarchive(config);
    }
    configFile.close();
}

std::string AssetManager::GetAssetPathByName(std::string name) {
    // Since we can call this from lua, there is some leniency about name referring to an actual file.
    auto asset = MicrogolfFilePaths.find(name);
    if (asset == MicrogolfFilePaths.end()) {
        TraceLog(LOG_ERROR, "Requested asset, %s, not found!", name.c_str());
        return "";
    } else {
        return asset->second;
    }
}

void ConfigManager::SetActiveConfig(Config config) {
    this->activeConfig = config;
    ApplyActiveConfig();
}

void ConfigManager::ApplyActiveConfig() {
    int screenWidth = this->activeConfig.res.x;
    int screenHeight = this->activeConfig.res.y;

    // Only run if raylib has not cretaed a window yet.
    if (!IsWindowReady()) {
        InitWindow(screenWidth, screenHeight, "Microgolf");
        InitAudioDevice();
    }

    if (IsWindowFullscreen() != this->activeConfig.res.fullscreen) {
            ToggleFullscreen();
        }

    SetWindowMonitor(this->activeConfig.res.selectedMonitor);
    SetWindowSize(this->activeConfig.res.x, this->activeConfig.res.y);
    SetTargetFPS(this->activeConfig.res.targetFPS);
}

const Config * ConfigManager::GetActiveConfig() {
    return &this->activeConfig;
}
