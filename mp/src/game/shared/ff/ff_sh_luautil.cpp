#include "cbase.h"
#include "ff_sh_luautil.h"

#include "lua.hpp"

void LUAUTIL_RemoveVarsFromGlobal( lua_State *L, const char** ppszVars )
{
	for( int i=0; ppszVars[i] != NULL; ++i )
	{
		lua_pushnil(L);
		lua_setglobal(L, ppszVars[i]);
	}
}

void LUAUTIL_RemoveKeysFromTable( lua_State *L, const char *pszTableName, const char** ppszKeys )
{
	lua_getglobal(L, pszTableName);
	if (lua_type(L, -1) == LUA_TTABLE)
	{
		for( int i=0; ppszKeys[i] != NULL; ++i )
		{
			lua_pushstring(L, ppszKeys[i]);
			lua_pushnil(L);
			lua_settable(L, -3);
		}
	}
	lua_pop(L, 1);
}

void LUAUTIL_StackDump (lua_State *L) {
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		Msg("%d) ", i);
		switch (t) {
	
			case LUA_TSTRING:  /* strings */
			Msg("'%s'", lua_tostring(L, i));
			break;
	
			case LUA_TBOOLEAN:  /* booleans */
			Msg(lua_toboolean(L, i) ? "true" : "false");
			break;
	
			case LUA_TNUMBER:  /* numbers */
			Msg("%g", lua_tonumber(L, i));
			break;
	
			default:  /* other values */
			Msg("%s", lua_typename(L, t));
			break;
	
		}
		Msg("   ");  /* put a separator */
	}
	Msg("\n");  /* end the listing */
}