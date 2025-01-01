// TEST FOR BASIC STINKY FUNCTIONALITY

#include <enet/enet.h>
#include <iostream>
#include <raylib.h>
#include "packettypes.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/misc/cpp/imgui_stdlib.h"
#include "stinky/stinky.hpp"
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
            rlImGuiBegin();
            ClearBackground(WHITE);

            client->Recv();
            if (client->GetPeersVector().size() == 0) {
                client->AttemptConnect();
            } else {
            ImGui::Begin("Social");
            auto localPlayer = client->GetPlayersMap().at(client->GetPlayerId());
            ImGui::Text("%s [%u]", localPlayer.nickname.c_str(), client->GetPlayerId());
            if (ImGui::BeginTabBar("Social")) {
                if (ImGui::BeginTabItem("Players")) {

                    std::vector<std::string> playerListBoxVector;

                    for (auto item : client->GetPlayersMap()) {
                        playerListBoxVector.push_back(item.second.nickname);
                    }

                    static int playerlistbox_itemcurrent = 0;
                    ImGui::ListBox("##playerlistbox", &playerlistbox_itemcurrent,
                        [](void * data, int n, const char ** out_text)->bool {
                            const std::vector<std::string>* v = static_cast<std::vector<std::string>*>(data);
                            *out_text = v->at(n).c_str();
                            return true;
                        },
                        &playerListBoxVector,
                        playerListBoxVector.size()
                    );

                    static char nick[32] = "unnamed";
                    ImGui::InputText("##inputtext", nick, 31);
                    nick[31] = '\0';
                    if (ImGui::Button("Change Nickname")) {
                        client->FormatAndSend(MG_NICKNAME_CHANGE, client->GetPeersVector()[0], 32, reinterpret_cast<unsigned char *>(nick));
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Chat")) {
                    ImGui::Text("Test");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
                ImGui::End();
            }

            rlImGuiEnd();
            DrawText("chat client test", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    rlImGuiShutdown();

    CloseWindow();

    delete(client);

    return 0;
}
