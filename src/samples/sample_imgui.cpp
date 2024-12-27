// TEST FOR IMGUI

#include <raylib.h>
#include "../gui/style.hpp"
#include "../vendor/imgui/imgui.h"
#include "../rlImGui.h"

// Todo: Consider mapload as first argument
int main(void) {
    const int screenWidth = 640;
    const int screenHeight = 480;
    // the holy resolution

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Test");
    SetTargetFPS(60);

    rlImGuiSetup(true);
    ImGuiIO* io = &ImGui::GetIO();
    io->IniFilename = NULL;
    io->LogFilename = NULL;

    while (!WindowShouldClose()) {


        BeginDrawing();
        {
            rlImGuiBegin();
            ClearBackground(BLACK);

            SetupGuiStyle();

            bool open = true;
            ImGui::ShowDemoWindow(&open);
            rlImGuiEnd();
            DrawText("IMGUI TEST", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    rlImGuiShutdown();

    CloseWindow();
    return 0;
}
