#include <SDL.h>
#include <string>

#include "LuaFunctions.h"
#include "LuaHandler.h"
#include "EntityHandler.h"
#include "Direct3DHeader.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

#define getuserdata(state, ptr, index) static_cast<ptr> \
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
	EntityHandler *entityHandler = getuserdata(state, EntityHandler*, 1);
	float pos[3] = { 0,0,0 }, size[3] = { 0,0,0 };

	if (lua_istable(state, -1) && lua_istable(state, -2)) {
		loadSizeAndPosition(state, pos, size);
		entityHandler->addBlock(Vector3(pos[0], pos[1], pos[2]), Vector3(size[0], size[1], size[2]));
		lua_pop(state, 2); //pop the two tables
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

int LuaFunctions::addCollider(lua_State *state) {
	LuaHandler *handler = getuserdata(state, LuaHandler*, 1);

	if (lua_istable(state, -1)) {
		float pos[3] = { 0,0,0 }, size[3] = { 0,0,0 };
		char const *name = "", *id = "";

		lua_pushstring(state, "position");
		lua_gettable(state, -2);
		lua_pushstring(state, "size");
		lua_gettable(state, -3);

		if (lua_istable(state, -1) && lua_istable(state, -2)) {
			loadSizeAndPosition(state, pos, size);
		}
		lua_pop(state, 2);

		lua_pushstring(state, "name");
		lua_gettable(state, -2);

		if (lua_isstring(state, -1)) {
			name = lua_tostring(state, -1);
		}
		lua_pop(state, 1);

		lua_pushstring(state, "id");
		lua_gettable(state, -2);

		if (lua_isstring(state, -1)) {
			id = lua_tostring(state, -1);
		}
		lua_pop(state, 1);

		LuaHandler::Collider collider;
		collider.position = Vector3(pos[0], pos[1], pos[2]);
		collider.size = Vector3(size[0], size[1], size[2]);
		collider.name = name;
		collider.id = id;

		handler->addCollider(collider);
	}
	lua_pop(state, 1);

	return 0;
}

/* A helper method */
bool LuaFunctions::handleError(lua_State *state, int error) {
	if (error) {
		SDL_Log("Error: %s", lua_tostring(state, -1));
		lua_pop(state, 1);
		return false;
	}

	return true;
}

void LuaFunctions::loadSizeAndPosition(lua_State *state, float pos[3], float size[3]) {
	std::string var[3] = { "x", "y", "z" };

	for (int i = 0; i < 3; i++) {
		lua_pushstring(state, var[i].c_str());
		lua_gettable(state, -2);
		if (lua_isnumber(state, -1)) {
			size[i] = lua_tonumber(state, -1);
		}
		lua_pop(state, 1);
	}

	for (int i = 0; i < 3; i++) {
		lua_pushstring(state, var[i].c_str());
		lua_gettable(state, -3);
		if (lua_isnumber(state, -1)) {
			pos[i] = lua_tonumber(state, -1);
		}
		lua_pop(state, 1);
	} 
}