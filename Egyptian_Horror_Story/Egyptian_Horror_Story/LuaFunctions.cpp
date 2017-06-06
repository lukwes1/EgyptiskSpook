#include <SDL.h>
#include <string>

#include "LuaFunctions.h"
#include "LuaHandler.h"
#include "EntityHandler.h"
#include "Direct3DHeader.h"
#include "SimpleMath.h"
#include "EntityHandler.h"

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

	if (lua_istable(state, -4) && lua_istable(state, -3)
		&& lua_isboolean(state, -2) && lua_isnumber(state, -1)) { // lua_isinteger doesnt work`? :/
		int texId = lua_tointeger(state, -1); //to does tho :)
		lua_pop(state, 1);
		bool solid = lua_toboolean(state, -1);
		lua_pop(state, 1);

		loadSizeAndPosition(state, pos, size);
		Vector3 pos(pos[0], pos[1], pos[2]);
		Vector3 size(size[0], size[1], size[2]);

		int gId = entityHandler->addBlock(pos, size,
			new AABB(pos, size), solid, texId);
		lua_pop(state, 2); //pop the two tables
		lua_pushnumber(state, gId);

		return 1;
	} else
		return 0;
}

int LuaFunctions::drawBuildingBlock(lua_State *state) {
	EntityHandler *entityHandler = getuserdata(state, EntityHandler*, 1);
	float pos[3] = { 0,0,0 }, size[3] = { 0,0,0 };

	if (lua_istable(state, -3) && lua_istable(state, -2) && lua_isnumber(state, -1)) { // lua_isinteger doesnt work`? :/
		int texId = lua_tointeger(state, -1);
		lua_pop(state, 1);

		loadSizeAndPosition(state, pos, size);
		Vector3 pos(pos[0], pos[1], pos[2]);
		Vector3 size(size[0], size[1], size[2]);

		int gId = entityHandler->updateBuildBlock(pos, size, texId);
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

	if (lua_istable(state, -2) && lua_isnumber(state, -1) ) {
		int gId = lua_tonumber(state, -1);
		lua_pop(state, 1);

		/** LOAD POSITION AND SIZE VECTORS */
		float pos[3] = { 0,0,0 }, size[3] = { 0,0,0 };
		char const *name = "";

		lua_pushstring(state, "position");
		lua_gettable(state, -2);
		lua_pushstring(state, "size");
		lua_gettable(state, -3);

		if (lua_istable(state, -1) && lua_istable(state, -2)) {
			loadSizeAndPosition(state, pos, size);
		}
		lua_pop(state, 2);

		/** LOAD NAME AND ID*/
		lua_pushstring(state, "name");
		lua_gettable(state, -2);

		if (lua_isstring(state, -1)) {
			name = lua_tostring(state, -1);
		}
		lua_pop(state, 1);

		LuaHandler::Collider collider;
		collider.position = Vector3(pos[0], pos[1], pos[2]);
		collider.size = Vector3(size[0], size[1], size[2]);
		collider.name = name;
		collider.gId = gId;

		lua_pushstring(state, "id");
		lua_gettable(state, -2);

		if (lua_isstring(state, -1))
			collider.id = lua_tostring(state, -1);
		else if (lua_isnumber(state, -1)) // A Bad solution for a bad problem
			collider.id = std::to_string(static_cast<int> (lua_tonumber(state, -1)));
		lua_pop(state, 1);
		
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

int LuaFunctions::hide(lua_State *state) {
	EntityHandler *entities = getuserdata(state, EntityHandler*, 1);
	
	if (lua_isinteger(state, -1)) {
		int id = lua_tointeger(state, -1);
		Entity *entity = entities->getEntity(id);

		entity->setScale(Vector3::Zero);
	}

	return 0;
}

int LuaFunctions::setPosition(lua_State *state) {
	EntityHandler *entities = getuserdata(state, EntityHandler*, 1);

	if (lua_isinteger(state, -4) && lua_isnumber(state, -3) &&
		lua_isnumber(state, -2) && lua_isnumber(state, -1)) {
		int id = lua_tointeger(state, -4);
		Entity *entity = entities->getEntity(id);

		float x = lua_tonumber(state, -3);
		float y = lua_tonumber(state, -2);
		float z = lua_tonumber(state, -1);

		lua_pop(state, 4);
		entity->setPosition(Vector3(x, y, z));
	}
	return 0;
}

int LuaFunctions::getPosition(lua_State *state) {
	EntityHandler *entities = getuserdata(state, EntityHandler*, 1);

	if (lua_isinteger(state, -1)) {
		int id = lua_tointeger(state, -1);
		Entity *entity = entities->getEntity(id);
		lua_pop(state, 1);
		
		Vector3 pos = entity->getPosition();
		lua_pushnumber(state, pos.x);
		lua_pushnumber(state, pos.y);
		lua_pushnumber(state, pos.z);
		return 3;
	}

	return 0;
}

int LuaFunctions::getPlayerDir(lua_State *state) {
	Player *player = getuserdata(state, Player*, 1);

	Vector3 forward = player->getCamera()->getForward();
	lua_pushnumber(state, forward.x);
	lua_pushnumber(state, forward.y);
	lua_pushnumber(state, forward.z);

	return 3;
}

int LuaFunctions::getPlayerPos(lua_State *state) {
	Player *player = getuserdata(state, Player*, 1);

	Vector3 pos = player->getPosition();
	lua_pushnumber(state, pos.x);
	lua_pushnumber(state, pos.y);
	lua_pushnumber(state, pos.z);

	return 3;
}

int LuaFunctions::setPlayerPos(lua_State *state) {
	Player *player = getuserdata(state, Player*, 1);
	if (lua_isnumber(state, -1) && lua_isnumber(state, -2)
		&& lua_isnumber(state, -3)) {
		float x = lua_tonumber(state, -3);
		float y = lua_tonumber(state, -2);
		float z = lua_tonumber(state, -1);
		lua_pop(state, 3);
		player->setPosition(Vector3(x, y, z));
	}
	return 0;

}

int LuaFunctions::setSpeed(lua_State *state) {
	Player *player = getuserdata(state, Player*, 1);
	
	if (lua_isnumber(state, -1)) {
		float speed = static_cast<float> (lua_tonumber(state, -1));
		player->setSpeed(speed);
	}

	return 0;

}


int LuaFunctions::resetSpeed(lua_State *state) {
	Player *player = getuserdata(state, Player*, 1);

	player->setSpeed();

	return 0;
}

int LuaFunctions::isInAir(lua_State *state) {
	Player *player = getuserdata(state, Player*, 1);
	lua_pushboolean(state, player->isJumping());
	return 1;
}

int LuaFunctions::isSprinting(lua_State *state) {
	Player *player = getuserdata(state, Player*, 1);
	lua_pushboolean(state, player->isRunning());
	return 1;
}