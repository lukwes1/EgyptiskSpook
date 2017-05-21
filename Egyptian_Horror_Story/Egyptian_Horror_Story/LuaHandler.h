#ifndef LUA_HANDLER
#define LUA_HANDLER

#include "Player.h"
#include "Direct3DHeader.h"
#include "SimpleMath.h"
#include <vector>
#include "lua.hpp"

class LuaHandler
{
public:
	struct Collider {
		char const *id;
		DirectX::SimpleMath::Vector3 position, size;
		char const *name;
		AABB *aabb = nullptr;
	};

	lua_State *state;
private:
	std::vector<Collider> colliders;
public:
	LuaHandler();
	~LuaHandler();

	void addCollider(Collider const &collider);
	void update(Player *player);
	void loadLua(char const *filePath);
	
	void addLuaFunctions(lua_State *state) const;
};

#endif