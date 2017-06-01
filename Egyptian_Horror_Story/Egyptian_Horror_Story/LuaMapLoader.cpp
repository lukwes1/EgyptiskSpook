#include <SDL.h>

#include "LuaMapLoader.h"
#include "LuaFunctions.h"

#define PATH "scripts/"
#define L ".lua"

LuaMapLoader::LuaMapLoader()
{
	state = nullptr;
	mapLoaderLoaded = false;

	this->entities = nullptr;
	this->luaHandler = nullptr;
}

LuaMapLoader::~LuaMapLoader()
{
	if (state)
		lua_close(state);
}

void LuaMapLoader::setupMapLoader(EntityHandler *entities, LuaHandler *luaHandler) {
	this->entities = entities;
	this->luaHandler = luaHandler;

	state = luaL_newstate();
	luaL_openlibs(state);

	loadFunctions();

	int error = luaL_loadfile(state, PATH "mapLoader" L) ||
		lua_pcall(state, 0, 0, 0); //Load the file

	if (LuaFunctions::handleError(state, error))
		mapLoaderLoaded = true;
}

void LuaMapLoader::loadFunctions() {
	LuaFunctions::addFunction(state, LuaFunctions::Log, "Log");

	void *data[] = {entities};
	LuaFunctions::addFunctionClosure(state, LuaFunctions::drawBlock,
		"DrawBlock", data, ARRAYSIZE(data));
	data[0] = { luaHandler };
	LuaFunctions::addFunctionClosure(state, LuaFunctions::addCollider,
		"AddCollider", data, ARRAYSIZE(data));
}

void LuaMapLoader::loadMap(char const *path) {
	if (mapLoaderLoaded) {
		lua_getglobal(state, "loadMap");
		lua_pushstring(state, path);

		if (LuaFunctions::handleError(state, lua_pcall(state, 1, 0, 0))) {
			// map loaded! i hope.. ??

		}
	}
}

bool LuaMapLoader::isMapLoaderLoaded() {
	return mapLoaderLoaded;
}

void LuaMapLoader::handleMouseEvents(SDL_Scancode const &code,
									 DirectX::SimpleMath::Vector3 const &norm,
									 DirectX::SimpleMath::Vector3 const &pos) {
	switch (code) {
		case SDL_SCANCODE_Q: //--
			lua_getglobal(state, "decreaseRange");
			LuaFunctions::handleError(state, lua_pcall(state, 0, 0, 0));
			break;
		case SDL_SCANCODE_E: //++
			lua_getglobal(state, "increaseRange");
			LuaFunctions::handleError(state, lua_pcall(state, 0, 0, 0));
			break;
		case SDL_SCANCODE_B: // Build
			buildObject(norm, pos);
			break;
		case SDL_SCANCODE_G: // Switch Obj
			lua_getglobal(state, "switchObjectType");
			LuaFunctions::handleError(state, lua_pcall(state, 0, 0, 0));
			break;
	}
}

void LuaMapLoader::buildObject(DirectX::SimpleMath::Vector3 const &norm,
							   DirectX::SimpleMath::Vector3 const &pos) {
	lua_getglobal(state, "placeObject");

	lua_pushnumber(state, norm.x);
	lua_pushnumber(state, norm.y);
	lua_pushnumber(state, norm.z);

	lua_pushnumber(state, pos.x);
	lua_pushnumber(state, pos.y);
	lua_pushnumber(state, pos.z);

	LuaFunctions::handleError(state, lua_pcall(state, 6, 0, 0));
}