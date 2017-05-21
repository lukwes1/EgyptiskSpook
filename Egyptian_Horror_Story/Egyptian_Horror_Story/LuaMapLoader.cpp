#include <SDL.h>

#include "LuaMapLoader.h"
#include "LuaFunctions.h"

#define PATH "scripts/"
#define L ".lua"

LuaMapLoader::LuaMapLoader()
{
	state = nullptr;
	mapLoaderLoaded = false;
}

LuaMapLoader::~LuaMapLoader()
{
	if (state)
		lua_close(state);
}

void LuaMapLoader::setupMapLoader(EntityHandler *entities) {
	this->entities = entities;

	state = luaL_newstate();
	luaL_openlibs(state);

	loadFunctions();

	int error = luaL_loadfile(state, PATH "mapLoader" L) ||
		lua_pcall(state, 0, 0, 0); //Load the file

	if (handleError(state, error)) { // Handle error
		mapLoaderLoaded = true;
	}
}

void LuaMapLoader::loadFunctions() {
	LuaFunctions::addFunction(state, LuaFunctions::Log, "Log");

	void *data[] = {entities};
	LuaFunctions::addFunctionClosure(state, LuaFunctions::drawBlock,
		"DrawBlock", data, ARRAYSIZE(data));
}

void LuaMapLoader::loadMap(char const *path) {
	if (mapLoaderLoaded) {
		lua_getglobal(state, "loadMap");
		lua_pushstring(state, path);

		if (handleError(state, lua_pcall(state, 1, 0, 0))) {
			// map loaded! i hope.. ??

		}
	}
}

/* A helper method */
bool inline LuaMapLoader::handleError(lua_State *state, int error) {
	if (error) {
		SDL_Log("Error: %s", lua_tostring(state, -1));
		lua_pop(state, 1);
		return false;
	}

	return true;
}

bool LuaMapLoader::isMapLoaderLoaded() {
	return mapLoaderLoaded;
}