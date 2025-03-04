// Main interface file w/ raylib and jolt
#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/details/helpers.hpp>
#include <cstdlib>
#include <raylib.h>

struct Resolution {
    // Resolution is calculated by 640*multiplier, 480*multiplier
    // This should be unsigned int, but imgui doesn't support that...
    int selectedMonitor = 0;
    bool downscaling = false;
    int x = 1920;
    int y = 1080;
    int targetFPS = 60;
    bool fullscreen = false;
    template <class Archive>
        void serialize(Archive &archive) {
            archive(selectedMonitor, downscaling, x, y, fullscreen, targetFPS);
        }
};

// Serialize and Deserialize Config
struct Config {
    Resolution res;
    uint fontSize = 15;
    template <class Archive>
    void serialize(Archive &archive) {
        archive(res, fontSize);
    }
};

// Config functions live in here as well
class AssetManager {
    public:
        // Constructor checks for file presence, dtor not required.
        AssetManager();

        // Save a config to file.
        void SaveConfig(Config config);

        // Load a config from file.
        Config LoadConfig();

        // Look for an asset by name to retrieve path.
        std::string GetAssetPathByName(std::string);
    private:
        const std::unordered_map<std::string, std::string> MicrogolfFilePaths = {
            // The constructor will check that *ALL* these required assets are here.
            // If the files are moved around after, all bets are off!
            {"config", std::string(GetApplicationDirectory()) + "./assets/config.cfg"},
            {"menubg", std::string(GetApplicationDirectory()) + "./assets/out/mainmenu.png"},
        };
};

class ConfigManager {
    public:
        // Load a configuration to active configuration slot and applies it.
        void SetActiveConfig(Config);

        // Get the active configuration
        const Config * GetActiveConfig();
    private:
        Config activeConfig;
        void ApplyActiveConfig();

};
