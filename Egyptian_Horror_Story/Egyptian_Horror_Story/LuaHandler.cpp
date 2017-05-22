#include "LuaHandler.h"
#include "LuaFunctions.h"

LuaHandler::LuaHandler()
{
	state = nullptr;
}


LuaHandler::~LuaHandler()
{
	if (state) {
		assert(lua_gettop(state) == 0);

		lua_close(state);
		state = nullptr;
	}

	for (auto& collider : colliders) {
		if (collider.aabb)
			delete collider.aabb;
	}
}

void LuaHandler::addCollider(Collider const &collider) {
	colliders.push_back(collider);
	int i = colliders.size() - 1;
	colliders[i].aabb = new AABB(colliders[i].position, colliders[i].size);

	assert(state);

	lua_getglobal(state, "prefix");
	
	if (lua_isstring(state, -1)) {
		std::string method = lua_tostring(state, -1);
		method += collider.name;
		lua_getglobal(state, method.c_str());

		if (lua_isfunction(state, -1)) {
			lua_pushstring(state, collider.id);
			lua_pushinteger(state, collider.gId);
			lua_pcall(state, 2, 0, 0);
		}
	}

	lua_pop(state, 1);
}

void LuaHandler::setEntityHandler(EntityHandler *entities) {
	this->entities = entities;
}

void LuaHandler::setPlayer(Player *player) {
	this->player = player;
}

void LuaHandler::update() {
	if (state) {
		lua_getglobal(state, "update");
		LuaFunctions::handleError(state, lua_pcall(state, 0, 0, 0));

		for (auto& collider : colliders) {
			if (collider.aabb->aabbVSCapsule(*player->col)) {
				lua_getglobal(state, "onPlayerCollision");
				lua_pushstring(state, collider.id);
				LuaFunctions::handleError(state, lua_pcall(state, 1, 0, 0));
			}
		}
	}
}

void LuaHandler::loadLua(char const *filePath) {
	state = luaL_newstate();
	luaL_openlibs(state);
	addLuaFunctions(state);

	int error = luaL_loadfile(state, filePath) || lua_pcall(state, 0, 0, 0);

	LuaFunctions::handleError(state, error);
}

void LuaHandler::addLuaFunctions(lua_State *state) const {
	LuaFunctions::addFunction(state, LuaFunctions::Log, "Log");

	void *data[] = { entities };
	LuaFunctions::addFunctionClosure(state, LuaFunctions::hide, "hide", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::setPosition, "SetPosition", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::getPosition, "GetPosition", data, ARRAYSIZE(data));

	data[0] = player;
	LuaFunctions::addFunctionClosure(state, LuaFunctions::setPlayerPos, "SetPlayerPos", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::getPlayerDir, "GetPlayerDir", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::setPlayerPos, "SetPlayerPos", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::setSpeed, "SetSpeed", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::resetSpeed, "ResetSpeed", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::isInAir, "IsInAir", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::isSprinting, "IsSprinting", data, ARRAYSIZE(data));
	LuaFunctions::addFunctionClosure(state, LuaFunctions::getPlayerDir, "GetPlayerDir", data, ARRAYSIZE(data));
}