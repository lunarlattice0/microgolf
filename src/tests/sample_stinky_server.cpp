// TEST FOR BASIC STINKY FUNCTIONALITY

#include <enet/enet.h>
#include <iostream>
#include <raylib.h>
#include "packettypes.hpp"
#include "stinky/stinky.hpp"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/misc/cpp/imgui_stdlib.h"
#include "../rlImGui.h"
#include <sstream>
#include <vector>

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
            server->Recv();
            rlImGuiBegin();
            ClearBackground(WHITE);

            ImGui::Begin("Test");
            if (ImGui::Button("send test packet") && server->GetPeersSize() > 0) {
                std::string test("I am a test message.");
                unsigned char * test_uc = reinterpret_cast<unsigned char *>(test.data());
                server->FormatAndSend(MG_TEST, &server->GetPeers()[0], test.length() + 1, test_uc);
            }
            ImGui::End();

            rlImGuiEnd();
            DrawText("STINKY SERVER", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }

    rlImGuiShutdown();

    CloseWindow();

    delete(server);

    return 0;
}
