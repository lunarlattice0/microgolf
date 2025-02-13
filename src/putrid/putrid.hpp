// Main interface file w/ raylib and jolt
#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/details/helpers.hpp>
#include <cstdlib>
#include <raylib.h>

struct Resolution {
    // Resolution is calculated by 640*multiplier, 480*multiplier
    // This should be unsigned int, but imgui doesn't support that...
    int multiplier = 1;
    bool fullscreen = false;
    template <class Archive>
        void serialize(Archive &archive) {
            archive(multiplier, fullscreen);
        }
};

// Serialize and Deserialize Config
struct Config {
    Resolution res;
    template <class Archive>
    void serialize(Archive &archive) {
        archive(res);
    }
};

// Config functions live in here as well
class AssetManager {
    public:
        // Constructor checks for file presence, dtor not required.
        AssetManager();

        void SaveConfig(Config config);
        Config LoadConfig();
        std::string GetAssetPathByName(std::string);
    private:
        const std::unordered_map<std::string, std::string> MicrogolfFilePaths = {
            // The constructor will check that *ALL* these required assets are here.
            // If the files are moved around after, all bets are off!
            {"config", std::string(GetApplicationDirectory()) + "./assets/config.cfg"},
            {"menubg", std::string(GetApplicationDirectory()) + "./assets/out/mainmenu.png"},
        };
};
