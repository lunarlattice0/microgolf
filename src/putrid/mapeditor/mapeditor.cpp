#include "mapeditor/mapeditor.hpp"
#include "gui/mapeditor.hpp"
#include "luacode.h"
#include "luahelper/luahelper.hpp"
#include <memory>
#include <raylib.h>

MapEditor::MapEditor() {
    this->Cam = std::make_unique<Camera>();
    this->Cam->projection = CAMERA_PERSPECTIVE;
    this->Cam->position = {1.0f, 1.0f, 1.0f};
    this->Cam->target = {0.0f,0.0f,0.0f};
    this->Cam->up = {0.0f, 1.0f, 0.0f};
    this->Cam->fovy = 45.0f;
    DisableCursor();
    this->lh = std::make_unique<LuauHelper>();
}

MapEditor::~MapEditor() {
    EnableCursor();
}

const Camera * MapEditor::GetCamera() {
    return this->Cam.get();
}

void MapEditor::Loop() {
    lh->CallServiceCallbacks();
    ClearBackground(WHITE);
    DrawGrid(10, 1.0f);
    if (IsCursorHidden()) {
        UpdateCamera(this->Cam.get(), CAMERA_FREE);
    }
    if (IsKeyPressed(KEY_SLASH)) {
        IsCursorHidden() ? EnableCursor() : DisableCursor();
    }
}

void MapEditor::RunScript(std::string str) {
    lh->CompileAndRun("MapScript", str.c_str(), str.length() + 1);
}
