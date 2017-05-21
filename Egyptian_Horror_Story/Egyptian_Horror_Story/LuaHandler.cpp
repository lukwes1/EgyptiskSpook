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
			lua_pcall(state, 1, 0, 0);
		}
	}

	lua_pop(state, 1);
}

void LuaHandler::update(Player *player) {
	if (state) {
		lua_getglobal(state, "update");
		LuaFunctions::handleError(state, lua_pcall(state, 0, 0, 0));

		for (auto& collider : colliders) {
			if (collider.aabb->aabbVSCapsule(*player->col)) {
				lua_getglobal(state, "onPlayerCollision");
				LuaFunctions::handleError(state, lua_pcall(state, 0, 0, 0));
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
	//LuaFunctions::addFunctionClosure(state, LuaFunctions::hide, "hide");
}