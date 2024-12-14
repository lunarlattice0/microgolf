// TEST FOR LUA

#include "luacode.h"
#include "lualib.h"
#include "lua.h"

#include <raylib.h>

#include "../vendor/imgui/imgui.h"
#include "../rlImGui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <iostream>

#include "gui/imgui_lua.hpp"

static int test(lua_State *L) {
    std::cout << "Called from lua!" << std::endl;
    return 1;
}
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

    // oh dear god
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, test, "testCFunction");
    lua_setglobal(L, "testCFunction");

    lua_pushcfunction(L, ImGuiLuaBridge::Begin, "ImGuiBegin");
    lua_setglobal(L, "ImGuiBegin");

    lua_pushcfunction(L, ImGuiLuaBridge::Text, "ImGuiText");
    lua_setglobal(L, "ImGuiText");

    lua_pushcfunction(L, ImGuiLuaBridge::End, "ImGuiEnd");
    lua_setglobal(L, "ImGuiEnd");

    lua_pop(L, 1);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            rlImGuiBegin();
            ClearBackground(WHITE);
            ImGui::Begin("Test");
            static size_t bytecodeSize = 0;
            static std::string luaScript;
            static int runResult = 0;
            ImGui::Text("Lua Script:");
            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##", &luaScript, ImVec2(ImGui::GetWindowSize().x - 16, 256), flags, NULL, NULL);

            if (ImGui::Button("Compile Bytecode")) {
                char* bytecode = luau_compile(luaScript.c_str(), luaScript.size() + 1, NULL, &bytecodeSize);
                runResult = luau_load(L, "test", bytecode, bytecodeSize, 0);
                free(bytecode);
                if (runResult == 0) {
                    lua_pcall(L, 0, 0, 0);
                } else {
                    std::cerr << lua_tostring(L, -1) << std::endl;
                    lua_pop(L,1);
                }
            }
            /*
            char * bytecode = luau_compile("ImGuiText(\"i sent this from LUA\")", 18, NULL, &bytecodeSize);
            runResult = luau_load(L, "test", bytecode, bytecodeSize, 0);
            free(bytecode);
            lua_pcall(L, 0, 0, 0);
            */
            ImGui::Text("Check your console for output.");
            ImGui::End();
            rlImGuiEnd();
            DrawText("LUA TEST", 0, 0, 50, BLACK);
        }
        EndDrawing();
    }
    rlImGuiShutdown();

    CloseWindow();
    lua_close(L);
    return 0;
}
