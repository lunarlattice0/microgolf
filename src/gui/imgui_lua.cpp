#include "imgui_lua.hpp"
#include "imgui.h"
#include "lua.h"
#include "lualib.h"

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
