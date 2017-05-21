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
	static int addCollider(lua_State *state);
	static bool handleError(lua_State *state, int error);

	// Wubbalubbadubdub
	static void loadSizeAndPosition(lua_State *state, float pos[3], float size[3]);
};

#endif