#ifndef LUAMAPLOADER_H
#define LUAMAPLOADER_H

#include "lua.hpp"

class LuaMapLoader
{
private:
	lua_State *state;
	bool mapLoaderLoaded;

	bool handleError(lua_State *state, int error);
public:
	LuaMapLoader();
	~LuaMapLoader();
	
	void setupMapLoader();
	void loadFunctions();
	void loadMap(char const *path);

	bool isMapLoaderLoaded();
};

#endif