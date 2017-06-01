#ifndef LUAMAPLOADER_H
#define LUAMAPLOADER_H

#include "lua.hpp"
#include "EntityHandler.h"
#include "LuaHandler.h"
#include <SDL.h>
#include <SimpleMath.h>

class LuaMapLoader
{
private:
	lua_State *state;
	EntityHandler *entities;
	LuaHandler *luaHandler;
	bool mapLoaderLoaded;

	void buildObject(DirectX::SimpleMath::Vector3 const &norm,
					 DirectX::SimpleMath::Vector3 const &pos);
public:
	LuaMapLoader();
	~LuaMapLoader();
	
	void setupMapLoader(EntityHandler *entities, LuaHandler *luaHandler);
	void loadFunctions();
	void loadMap(char const *path);
	void handleMouseEvents(SDL_Scancode const &code,
						   DirectX::SimpleMath::Vector3 const &norm,
						   DirectX::SimpleMath::Vector3 const &pos);

	bool isMapLoaderLoaded();
};

#endif