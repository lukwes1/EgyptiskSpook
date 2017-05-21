#include "LuaFunctions.h"
#include <SDL.h>
#include <string>

LuaFunctions::LuaFunctions()
{
}


LuaFunctions::~LuaFunctions()
{
}

int LuaFunctions::Log(lua_State *state) {
	int args = lua_gettop(state);
	std::string print = "";

	for (int i = 0; i < args; i++) {
		if (lua_isstring(state, -1)) {
			print = lua_tostring(state, -1) + print;
			lua_pop(state, 1);
		}
	}

	SDL_Log(print.c_str());
	return 0;
}


void LuaFunctions::addFunction(lua_State *state,
	lua_CFunction const &function, char const *name) {
	lua_pushcfunction(state, function);
	lua_setglobal(state, name);
}