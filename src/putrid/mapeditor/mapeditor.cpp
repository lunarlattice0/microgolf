#include "mapeditor/mapeditor.hpp"
#include "luahelper/luahelper.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/details/helpers.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <raylib.h>
#include <stdexcept>

void MapEditor::LoadMap(const char * path) {
    if (this->activeMap != nullptr) {
        this->activeMap.reset(); // TODO: Prompt the user to save.
    }

    Map test;
    if (std::filesystem::exists(path)) {
        // Attempt to load from file.
        std::ifstream mapFile(path);
        try {
            {
                cereal::JSONInputArchive iarchive(mapFile);
                iarchive(test);
            }
        } catch (const cereal::Exception& e) {
            throw std::runtime_error("Failed to open map.");
        }
    }
}
/*
void MapEditor::SaveMap(const char * path) {
    std::ofstream mapFile(path);
    {
        cereal::JSONOutputArchive oarchive(mapFile);
        oarchive(this->activeMap);
    }
    }*/

MapEditor::MapEditor() {
    this->activeMap = nullptr;
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
