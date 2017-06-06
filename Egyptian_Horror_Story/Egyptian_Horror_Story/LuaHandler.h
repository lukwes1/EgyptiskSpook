#ifndef LUA_HANDLER
#define LUA_HANDLER

#include "Player.h"
#include "Direct3DHeader.h"
#include "SimpleMath.h"
#include <vector>
#include "lua.hpp"
#include "EntityHandler.h"

class LuaHandler
{
public:
	struct Collider {
		int gId;
		std::string id;
		DirectX::SimpleMath::Vector3 position, size;
		char const *name;
		AABB *aabb = nullptr;
	};

	lua_State *state;
	EntityHandler *entities;
	Player *player;
private:
	std::vector<Collider> colliders;
public:
	LuaHandler();
	~LuaHandler();

	void setEntityHandler(EntityHandler *entities);
	void setPlayer(Player *player);

	void addCollider(Collider const &collider);
	void update();
	void loadLua(char const *filePath);
	
	void addLuaFunctions(lua_State *state) const;
};

#endif