#ifndef LUAMAPLOADER_H
#define LUAMAPLOADER_H

#include "lua.hpp"
#include "EntityHandler.h"
#include "LuaHandler.h"

class LuaMapLoader
{
private:
	lua_State *state;
	EntityHandler *entities;
	LuaHandler *luaHandler;
	bool mapLoaderLoaded;
public:
	LuaMapLoader();
	~LuaMapLoader();
	
	void setupMapLoader(EntityHandler *entities, LuaHandler *luaHandler);
	void loadFunctions();
	void loadMap(char const *path);

	bool isMapLoaderLoaded();
};

#endif