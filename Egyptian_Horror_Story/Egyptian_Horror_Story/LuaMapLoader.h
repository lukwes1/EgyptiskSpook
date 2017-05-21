#ifndef LUAMAPLOADER_H
#define LUAMAPLOADER_H

#include "lua.hpp"
#include "EntityHandler.h"

class LuaMapLoader
{
private:
	lua_State *state;
	EntityHandler *entities;
	bool mapLoaderLoaded;

	bool handleError(lua_State *state, int error);
public:
	LuaMapLoader();
	~LuaMapLoader();
	
	void setupMapLoader(EntityHandler *entities);
	void loadFunctions();
	void loadMap(char const *path);

	bool isMapLoaderLoaded();
};

#endif