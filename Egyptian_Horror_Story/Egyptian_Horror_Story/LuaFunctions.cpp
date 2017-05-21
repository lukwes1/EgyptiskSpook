#include "LuaFunctions.h"
#include <SDL.h>
#include <string>
#include "EntityHandler.h"

#include "Direct3DHeader.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

#define touserdata(state, ptr, index) static_cast<ptr> \
		(lua_touserdata(state, lua_upvalueindex(index))) 

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

int LuaFunctions::drawBlock(lua_State *state) {
	EntityHandler *entityHandler = touserdata(state, EntityHandler*, 1);
	std::string var[3] = { "x", "y", "z" };
	float f[3] = { 0, 0, 0 };

	if (lua_istable(state, -1) && lua_istable(state, -2)) {
		for (int i = 0; i < 3; i++) {
			lua_pushstring(state, var[i].c_str());
			lua_gettable(state, -2);
			if (lua_isnumber(state, -1)) {
				f[i] = lua_tonumber(state, -1);
			}
			lua_pop(state, 1);
		}
		Vector3 size(f[0], f[1], f[2]);
		lua_pop(state, 1);

		for (int i = 0; i < 3; i++) {
			lua_pushstring(state, var[i].c_str());
			lua_gettable(state, -2);
			if (lua_isnumber(state, -1)) {
				f[i] = lua_tonumber(state, -1);
			}
			lua_pop(state, 1);
		}
		Vector3 position(f[0], f[1], f[2]);
		lua_pop(state, 1);

		entityHandler->addBlock(position, size);
	}

	return 0;
}

void LuaFunctions::addFunction(lua_State *state,
	lua_CFunction const &function, char const *name) {
	lua_pushcfunction(state, function);
	lua_setglobal(state, name);
}

void LuaFunctions::addFunctionClosure(lua_State *state,
	lua_CFunction const &function, char const *name,
	void *ptrs[], int size) {
	for (int i = 0; i < size; i++)
		lua_pushlightuserdata(state, ptrs[i]);

	lua_pushcclosure(state, function, size);
	lua_setglobal(state, name);
}