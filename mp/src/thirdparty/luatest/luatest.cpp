// Lua includes
#include "lua.hpp"
//#include "luabind/luabind.hpp"
//#include <cstdio>

lua_State *L;

// redirect Lua's print function to output to the console
int print(lua_State *L)
{
	return lua_status(L);
	//lua_gettop(L);
	/*int i;
	for (i=1; i<=n; i++)
	{
	if (i==0) printf("[Lua] ");
	if (i>1) printf("\t");
	if (lua_isstring(L,i))
		printf("%s",lua_tostring(L,i));
	else if (lua_isnil(L,i))
		printf("%s","nil");
	else if (lua_isboolean(L,i))
		printf("%s",lua_toboolean(L,i) ? "true" : "false");
	else
		printf("%s:%p",luaL_typename(L,i),lua_topointer(L,i));
	}
	printf("\n");*/
}

/*
int main(int argc, char **argv)
{
	return 0;
}*/