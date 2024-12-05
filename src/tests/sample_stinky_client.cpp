// TEST FOR BASIC STINKY FUNCTIONALITY

#include <enet/enet.h>
#include <exception>
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

    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = 6969;
    Stinky::Client * client = new Stinky::Client(&address, 1, 8, 0);
    client->PrepareConnect();

    bool exit_flag = false;
    std::thread connectionAttempt([&client, &exit_flag]{
        do {
            client->RecvLoop(500);
        } while (client->GetPeers()->size() == 0 && exit_flag == false);
        TraceLog(LOG_INFO, "Got connection.");
    });
    while (!WindowShouldClose()) {
        client->RecvLoop(0);
        BeginDrawing();
        {
            rlImGuiBegin();
            ClearBackground(WHITE);

            rlImGuiEnd();
            DrawText("STINKY CLIENT", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    exit_flag = true;
    connectionAttempt.join();
    rlImGuiShutdown();

    CloseWindow();

    client->Cleanup();
    delete(client);


    return 0;
}
