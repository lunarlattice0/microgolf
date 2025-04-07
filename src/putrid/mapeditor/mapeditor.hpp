#pragma once
#include "putrid.hpp"
#include "raylib.h"

class MapEditor {
    public:
        MapEditor();
        ~MapEditor();
        const Camera * GetCamera();

        // Call this from within Raylib's loop.
        void Loop();
    private:
        // Using flycam for mapitor
        Camera * Cam;
};
