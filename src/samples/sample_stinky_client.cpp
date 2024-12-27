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

    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = 6969;
    Stinky::Client * client = new Stinky::Client(&address, 1, 8, 0);
    client->AttemptConnect();
    while (!WindowShouldClose()) {
        // TODO: Track time with epoch to determine when to stop trying to connect.
        BeginDrawing();
        {
            client->Recv();
            if (client->GetPeersSize() == 0) {
                client->AttemptConnect();
            }
            rlImGuiBegin();
            ClearBackground(WHITE);
            //for (const auto & [key, value] : *client->GetPlayerList()) {
            //    std::cout << key << ": " << value.nickname << std::endl;
            //}
            rlImGuiEnd();
            DrawText("STINKY CLIENT", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    rlImGuiShutdown();

    CloseWindow();

    delete(client);


    return 0;
}
