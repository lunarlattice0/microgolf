#include "luahelper.hpp"
#include "luacode.h"
#include "lualib.h"
#include "lua.h"
#include <cstddef>
#include <cstdlib>
#include <raylib.h>

// Note (Apr 19, 2025):
// 1)
// it seems luau does not support storing references outside of LUA_REGISTRYINDEX...
// 2)
// The compiler will not be happy if you pop a lua string BEFORE a function call using that string (even if you make a new var).
// This is not solvable with `volatile` since std::string cannot be marked `volatile`.
// ...Just pop after the function call...
// Addendum:
// Do NOT use std::string created from lua's const char *. I don't think the compiler manages std::string's lifetime properly, and its dtors may not be triggered
// properly, leaking a ton of memory.
// TLDR: the compiler is optimizing out the deep copy for reference swapping instead because it doesn't know about lua_pop
// and then the string becomes SILENTLY invalid when passed to another function

// As suggested by slappy, use tagged userdata instead
// faster?
#define RENDERSVC_TAG 2

// Lua function definitions

// Overloaded print
static int print(lua_State *L) {
  int nargs = lua_gettop(L);
  std::stringstream luaPrintStr;
  for (int i = 1; i <= nargs; ++i) {
    luaPrintStr << luaL_tolstring(L, i, nullptr);
    lua_pop(L, 1);
    if (i <= nargs - 1) {
      luaPrintStr << "\t";
    }
  }

  std::string concat = "Lua: " + luaPrintStr.str();
  TraceLog(LOG_INFO, concat.c_str());
  return 0;
}
//

// Render Services
// Initialization done in ctor of luahelper.

void RenderSvc::AddTask(int ref, const char * fname) {
    this->activeCallbacks[fname] = ref;
}
void RenderSvc::RemoveTask(const char * fname) {
    this->activeCallbacks.erase(fname);
}

int RenderSvc::GetTask(const char * fname) {
    auto idx = this->activeCallbacks.find(fname);
    if (idx != activeCallbacks.end()) {
        // Return IDX if found
        return idx->second;
    } else {
        // Return null if not
        return 0;
    }
}

void RenderSvc::CallRenderCallbacks(lua_State *L) {
    for (auto it : this->activeCallbacks) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, it.second);
        lua_pushnumber(L, GetFrameTime());
        lua_pcall(L, 1, 0, 0);
    }
}

// Callback's first arg will be delta time.
// Syntax is RenderSvc:AddTask(function, "function_name")
static int RenderSvc_AddTask(lua_State *L) {

    if (lua_gettop(L) == 3 && lua_isfunction(L, 2) && lua_isstring(L, 3)) {
        auto fname = lua_tostring(L, 3);
        int ref = lua_ref(L, -2);

        // Destroy the existing task (unsure if this solves a memory leak?)
        if ((*static_cast<RenderSvc**>(lua_touserdatatagged(L, 1, RENDERSVC_TAG)))->GetTask(fname) != 0) {
            (*static_cast<RenderSvc**>(lua_touserdatatagged(L, 1, RENDERSVC_TAG)))->RemoveTask(fname);
        }

        (*static_cast<RenderSvc**>(lua_touserdatatagged(L, 1, RENDERSVC_TAG)))->AddTask(ref, fname);
        lua_pop(L, -1);
        lua_pop(L, -1);
    }
    return 0;
}

// Syntax is RenderSvc:RemoveTask("function_name")
static int RenderSvc_RemoveTask(lua_State *L) {
    if (lua_gettop(L) == 2 && lua_isstring(L, 2)) {
        lua_pushvalue(L, -1);
        int ref = (*reinterpret_cast<RenderSvc**>(lua_touserdatatagged(L, 1, RENDERSVC_TAG)))->GetTask(lua_tostring(L, -1));
        (*reinterpret_cast<RenderSvc**>(lua_touserdatatagged(L, 1, RENDERSVC_TAG)))->RemoveTask(lua_tostring(L, -1));
        lua_unref(L, ref);
    }
    return 0;
}
// End lua definitions


// Initialize a LuaU VM.
LuauHelper::LuauHelper() {
    this->L = luaL_newstate();
    luaL_openlibs(this->L);

    // Print
    lua_pushcfunction(L, print, "print");
    lua_setglobal(L, "print");

    // RenderSvc
    // i love u stack overflow
    // https://stackoverflow.com/questions/22515908/using-straight-lua-how-do-i-expose-an-existing-c-class-objec-for-use-in-a-lua
    //*static_cast<RenderSvc**>(lua_newuserdatadtor(L, sizeof(RenderSvc*), [](void * rs) -> void {
    //    delete *static_cast<RenderSvc**>(rs);
    //})) = new RenderSvc();
    *static_cast<RenderSvc**>(lua_newuserdatatagged(L, sizeof(RenderSvc*), RENDERSVC_TAG)) = new RenderSvc();
    lua_setuserdatadtor(L, RENDERSVC_TAG, [](lua_State *L, void * rs) -> void {
        (void)L;
        delete * static_cast<RenderSvc**>(rs);
    });

    luaL_newmetatable(L, "RenderSvc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, RenderSvc_AddTask, "RenderSvc_AddTask");
    lua_setfield(L, -2, "AddTask");

    lua_pushcfunction(L, RenderSvc_RemoveTask, "RenderSvc_RemoveTask");
    lua_setfield(L, -2, "RemoveTask");

    lua_setmetatable(L, -2);
    lua_pushvalue(L, -1);
    this->renderSvc = (*reinterpret_cast<RenderSvc**>(lua_touserdatatagged(L, 1, RENDERSVC_TAG)));
    lua_setglobal(L, "RenderSvc");
}

// Compile a lua file into bytecode and run it.
// Returns true upon success, and false upon failure.
bool LuauHelper::CompileAndRun(const char * name, const char * source, int sourceLen) {
    size_t bytecodeSize = 0;
    char * bytecode = luau_compile(source, sourceLen, NULL, &bytecodeSize);
    int runResult = 0;
    runResult = luau_load(L, name, bytecode, bytecodeSize, 0);
    free(bytecode);
    if (runResult == 0) {
        lua_pcall(L, 0, 0, 0);
        return true;
    } else {
        TraceLog(LOG_INFO, lua_tostring(L, -1));
        lua_pop(L,1);
        return false;
    }
}

void LuauHelper::CallServiceCallbacks() {
    this->renderSvc->CallRenderCallbacks(this->L);
}

LuauHelper::~LuauHelper() {
    lua_close(this->L);
}
