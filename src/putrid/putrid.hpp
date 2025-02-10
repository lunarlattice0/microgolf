// Main interface file w/ raylib and jolt
// Try to keep this file free of OOP for easy compatibility w/ lua.

#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/details/helpers.hpp>
#include <cstdlib>
#include <filesystem>
#include <raylib.h>

// initialized in putrid.cpp
extern const std::unordered_map<std::string, std::string> MicrogolfFilePaths;

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
Config LoadConfigFromFile(const char * filepath);
void SaveConfigToFile(const char * filepath, Config config);

// Check if the following resources are available; if not, crash
// For compatibility with windows & macos, store assets in the same folder as microgolf
void CheckFilePaths();
