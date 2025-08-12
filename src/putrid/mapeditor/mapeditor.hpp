#pragma once
#include "luahelper/luahelper.hpp"
#include "world/world.hpp"
#include "raylib.h"
#include <cereal/archives/json.hpp>


struct Map {
    std::string script; // The game script for the level.
    std::vector<Entity> entityVec; // The entities in the level.
    Vector3 startLocation;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(script), CEREAL_NVP(entityVec));//, CEREAL_NVP(startLocation));
    }
};

class MapEditor {
    public:
        void LoadMap(const char * path);
        void SaveMap(const char * path);
        MapEditor();
        ~MapEditor();
        const Camera * GetCamera();

        // Call this from within Raylib's loop.
        void Loop();
        void RunScript(std::string str);

    private:
        // Using flycam for mapitor
        std::unique_ptr<Map> activeMap;
        std::unique_ptr<Camera> Cam;
        std::unique_ptr<LuauHelper> lh;
};
