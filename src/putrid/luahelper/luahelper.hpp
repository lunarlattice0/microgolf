// Interface with luau helper functions

#pragma once
#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <raylib.h>
#include <sstream>
#include <vector>
//#include "imgui_lua.hpp"

// Override the print() in lua so that it is nice with raylib

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

// Totally not a ripoff of RunService...
class RenderSvc {
    private:
        //std::vector<typename Tp> tasks;
    public:
        void AddTask();
        void RemoveTask();
};
