#pragma once
#include "luahelper/luahelper.hpp"
#include "putrid.hpp"
#include "raylib.h"

class MapEditor {
    public:
        MapEditor();
        ~MapEditor();
        const Camera * GetCamera();

        // Call this from within Raylib's loop.
        void Loop();
        void RunScript(std::string str);

    private:
        // Using flycam for mapitor
        std::unique_ptr<Camera> Cam;
        std::unique_ptr<LuauHelper> lh;
};
