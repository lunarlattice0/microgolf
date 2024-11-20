#include <raylib.h>

#include "vendor/imgui/imgui.h"
#include "rlImGui.h"

// Todo: Consider mapload as first argument
int main(void) {
    const int screenWidth = 640;
    const int screenHeight = 480;
    // the holy resolution

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Test");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            rlImGuiBegin();
            ClearBackground(WHITE);

            bool open = true;
            ImGui::ShowDemoWindow(&open);
            rlImGuiEnd();
            DrawText("Text", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    rlImGuiShutdown();

    CloseWindow();
    return 0;
}
