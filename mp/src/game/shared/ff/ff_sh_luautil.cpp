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