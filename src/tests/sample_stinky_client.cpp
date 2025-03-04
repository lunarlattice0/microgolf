// TEST FOR BASIC STINKY FUNCTIONALITY
// TODO: Figure out how to let the player know that there is a chat delay
#include <cstring>
#include <enet/enet.h>
#include <raylib.h>
#include <stdexcept>
#include <string>
#include "packettypes.hpp"
#include "vendor/imgui/imgui.h"
#include "stinky/stinky.hpp"
#include "src/vendor/rlImGui/rlImGui.h"
#include "gui/style.hpp"

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
        BeginDrawing();
        {
            rlImGuiBegin();
            ClearBackground(WHITE);

            client->Recv();
            if (client->GetPeersVector().size() == 0) {
                client->AttemptConnect();
            } else {
            SetupGuiStyle();
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
                        //client->FormatAndSend(MG_NICKNAME_CHANGE, client->GetPeersVector()[0], 32, reinterpret_cast<unsigned char *>(nick));
                        client->FormatAndSend<char>(MG_NICKNAME_CHANGE, client->GetPeersVector()[0], 32, nick);
                    }
                    ImGui::EndTabItem();
                }
                // TODO: Figure out how to hide the hover effect
                if (ImGui::BeginTabItem("Chat")) {
                    std::vector<std::string> formattedMessages;
                    for (auto it : client->GetMessageVector()) {
                        std::string nickname = "disconnected";
                        try {
                            auto messagePlayerSource = client->GetPlayersMap().at(it.lastChatMessageSource);
                            nickname = messagePlayerSource.nickname;
                        } catch (const std::out_of_range& e) {}
                        std::string formattedMessage = "[" + nickname + "]: " + it.lastChatMessage;
                        formattedMessages.push_back(formattedMessage);
                    }

                    static int chatbox_itemcurrent = 0; // don't use this value, the hovering effect is invisible.
                    ImGui::ListBox("##chatlistbox", &chatbox_itemcurrent,
                        [](void * data, int n, const char ** out_text)->bool {
                            const std::vector<std::string>* v = static_cast<std::vector<std::string>*>(data);
                            *out_text = v->at(n).c_str();
                            return true;
                        },
                        &formattedMessages,
                        formattedMessages.size()
                    );
                    static char msg[256] = "";
                    ImGui::InputText("##chatinputtext", msg, 31);
                    msg[255] = '\0';
                    if (ImGui::Button("Send Message")) {

                        //client->FormatAndSend(MG_CHAT, client->GetPeersVector()[0], 256, reinterpret_cast<unsigned char *>(msg));
                        client->FormatAndSend<char>(MG_CHAT, client->GetPeersVector()[0], 256, msg);
                        std::memset(msg, '\0', 256);
                    }

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
