#ifndef LUA_FUNCTIONS
#define LUA_FUNCTIONS

#include "Lua.hpp"

class LuaFunctions
{
public:
	LuaFunctions();
	~LuaFunctions();

	static void addFunction(lua_State *state,
		lua_CFunction const &function, char const *name);
	static void addFunctionClosure(lua_State *state,
		lua_CFunction const &function, char const *name,
		void *ptrs[], int size);

	static int Log(lua_State *state);
	static int drawBlock(lua_State *state);
	static int drawBuildingBlock(lua_State *state);
	static int addCollider(lua_State *state);
	static bool handleError(lua_State *state, int error);

	static int hide(lua_State *state);
	static int setPosition(lua_State *state);
	static int getPosition(lua_State *state);

	static int getPlayerPos(lua_State *state); 
	static int setPlayerPos(lua_State *state);
	static int setSpeed(lua_State *state);
	static int resetSpeed(lua_State *state);
	static int isInAir(lua_State *state);
	static int isSprinting(lua_State *state);
	static int getPlayerDir(lua_State *state);

	static int victory(lua_State *state);
	// Wubbalubbadubdub
	static void loadSizeAndPosition(lua_State *state, float pos[3], float size[3]);
};

#endif