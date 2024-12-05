// TEST FOR BASIC STINKY FUNCTIONALITY

#include <enet/enet.h>
#include <raylib.h>
#include "stinky/stinky.hpp"
#include "../vendor/imgui/imgui.h"
#include "../rlImGui.h"

int main(void) {
    const int screenWidth = 640;
    const int screenHeight = 480;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Test");
    SetTargetFPS(60);

    rlImGuiSetup(true);
    ImGuiIO* io = &ImGui::GetIO();
    io->IniFilename = NULL;
    io->LogFilename = NULL;

    ENetAddress adr;
    adr.host = ENET_HOST_ANY;
    adr.port = 6969;

    Stinky::Server * server = new Stinky::Server(adr, 8, 8, 0);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            server->RecvLoop();
            rlImGuiBegin();
            ClearBackground(WHITE);

            rlImGuiEnd();
            DrawText("STINKY SERVER", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    server->Cleanup();

    rlImGuiShutdown();

    CloseWindow();

    delete(server);

    return 0;
}
