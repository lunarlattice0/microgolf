// Interface with luau helper functions

#pragma once
#include "lua.h"
//#include "imgui_lua.hpp"

class LuauHelper {
    public:
        LuauHelper();
        bool CompileAndRun(const char * name, const char * source, int sourceLen);
        //static int CompileAndRun(lua_State *L);
        ~LuauHelper();
    private:
        lua_State *L;
//        ImGuiLuaBridge * lb;
};
