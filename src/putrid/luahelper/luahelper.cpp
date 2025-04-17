#include "luahelper.hpp"
#include "luacode.h"
#include "lualib.h"
#include "lua.h"
#include <cstdlib>
#include <raylib.h>

// TODO: Set up a Render() function to be called from Raylib

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

static int RenderSvc_new(lua_State *L) {
    luaL_newmetatable(L, "RenderSvc");
    *reinterpret_cast<RenderSvc**>(lua_newuserdata(L, sizeof(RenderSvc*))) = new RenderSvc();
    lua_setmetatable(L, -2); // TODO: Unsure about this...

    return 1;
}

/*
Why think when you could use lambdas....
void RenderSvc_delete(void * rs) {
    delete *reinterpret_cast<RenderSvc**>(rs);
    // are you feeling it now mr krabs
}
*/

static int RenderSvc_AddTask(lua_State *L) {
    // STUB
    std::cout << "Addtask called!" << std::endl;
    return 0;
}

static int RenderSvc_RemoveTask(lua_State *L) {
    // STUB
    std::cout << "Removetask called!" << std::endl;
    return 0;
}
//

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
    *reinterpret_cast<RenderSvc**>(lua_newuserdatadtor(L, sizeof(RenderSvc*), [](void * rs) -> void {
        delete *reinterpret_cast<RenderSvc**>(rs);
    })) = new RenderSvc();

    luaL_newmetatable(L, "RenderSvc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, RenderSvc_AddTask, "RenderSvc_AddTask");
    lua_setfield(L, -2, "AddTask");

    lua_pushcfunction(L, RenderSvc_RemoveTask, "RenderSvc_RemoveTask");
    lua_setfield(L, -2, "RemoveTask");

    lua_setmetatable(L, -2);
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

LuauHelper::~LuauHelper() {
    lua_close(this->L);
}
