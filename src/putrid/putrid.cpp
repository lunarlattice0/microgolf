#include "putrid/putrid.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <raylib.h>
#include <fstream>
#include <filesystem>

// Overhaul this garbage, i hate it.
AssetManager::AssetManager() {
    for (auto it : MicrogolfFilePaths) {
        if (!std::filesystem::exists(it.second)) {
            if (it.second == MicrogolfFilePaths.at("config")) {
                TraceLog(LOG_INFO, "Missing config, will load a default...");
                continue;
            }
            TraceLog(LOG_ERROR, "Missing file: %s", it.second.c_str());
            std::exit(1);
        }
    }
}

Config ConfigManager::LoadConfig(AssetManager * am) {
    Config config;
    if (!std::filesystem::exists(am->GetAssetPathByName("config")) || std::filesystem::is_empty(am->GetAssetPathByName("config"))) {
        // Save the default conf
        SaveConfig(am, config);
        return config;
    }

    std::ifstream configFile(am->GetAssetPathByName("config"));
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

void ConfigManager::SaveConfig(AssetManager * am, Config config) {
    std::ofstream configFile(am->GetAssetPathByName("config"));
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

    // Disable exit by esc
    SetExitKey(0);

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
