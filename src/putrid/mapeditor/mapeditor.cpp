#include "mapeditor/mapeditor.hpp"
#include <raylib.h>

MapEditor::MapEditor() {
    this->Cam = Camera{};
    this->Cam.projection = CAMERA_PERSPECTIVE;
    this->Cam.position = {0.0f, 0.0f, 0.0f};
    this->Cam.target = {10.0f,10.0f,10.0f};
    this->Cam.up = {0.0f, 1.0f, 0.0f};
    this->Cam.fovy = 45.0f;
    DisableCursor();
}

MapEditor::~MapEditor() {
    this->Cam = {0};
    EnableCursor();
}

const Camera * MapEditor::GetCamera() {
    return &this->Cam;
}

void MapEditor::Loop() {
    ClearBackground(WHITE);
    DrawGrid(10, 1.0f);
    DrawCube({0,0,0}, 5.0f, 5.0f, 5.0f, RED);
    UpdateCamera(&this->Cam, CAMERA_FREE);
}
