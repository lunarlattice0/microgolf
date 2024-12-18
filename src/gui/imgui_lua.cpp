//
#include "imgui_lua.hpp"
#include "VM/src/lobject.h"
#include "imgui.h"
#include "lua.h"
#include "lualib.h"
#include "misc/cpp/imgui_stdlib.h"
#include <vector>
#include "iostream"
// TODO: I'm hella lazy, so I will be only implementing a subset of ImGui widgets.

ImGuiLuaBridge::ImGuiLuaBridge(lua_State *L) {
    luaL_register(L, "ImGuiLuaBridge", this->LuaFuncs);
}

/*
int ImGuiLuaBridge::Test(lua_State *L) {
    std::cout << "Test" << std::endl;
    return 0;
}
*/

// Window title CANNOT be empty.
int ImGuiLuaBridge::Begin(lua_State *L) {
    auto title = luaL_checkstring(L, -1);
    ImGui::Begin(title);
    return 0;
}

int ImGuiLuaBridge::End(lua_State *L) {
    ImGui::End();
    return 0;
}

int ImGuiLuaBridge::Text(lua_State *L) {
    auto text = luaL_checkstring(L, -1);
    ImGui::Text("%s", text);
    return 0;
}

int ImGuiLuaBridge::Button(lua_State *L) {
    auto text = luaL_checkstring(L, -1);
    if (ImGui::Button(text)) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

int ImGuiLuaBridge::InputTextMultiline(lua_State *L) {
    // args = label(string), text(buffer), returns buffer

    size_t bufferSize = 0;
    auto textBuffer = static_cast<char *>(luaL_checkbuffer(L, -1, &bufferSize));
    auto label = luaL_checkstring(L, -2);
    ImGui::InputTextMultiline(label, textBuffer, bufferSize);

    return 0;
}
