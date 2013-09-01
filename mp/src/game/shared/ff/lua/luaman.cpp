// luaman.cpp

//---------------------------------------------------------------------------
// includes
#include "cbase.h"

// Header
#include "lua/luaman.h"

// Lua includes
#include "lua.hpp"
#include "luabind/luabind.hpp"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

lua_State *L;

// redirect Lua's print function to output to the console
static int print(lua_State *L)
{
	int n=lua_gettop(L);
	int i;
	for (i=1; i<=n; i++)
	{
	if (i==0) Msg("[Lua] ");
	if (i>1) Msg("\t");
	if (lua_isstring(L,i))
		Msg("%s",lua_tostring(L,i));
	else if (lua_isnil(L,i))
		Msg("%s","nil");
	else if (lua_isboolean(L,i))
		Msg("%s",lua_toboolean(L,i) ? "true" : "false");
	else
		Msg("%s:%p",luaL_typename(L,i),lua_topointer(L,i));
	}
	Msg("\n");
	return 0;
}

void lua_init()
{
	// initialize lua
#if defined (CLIENT_DLL)
	Msg("[Lua] Initializing client-side Lua...\n");
#else
	Msg("[Lua] Initializing server-side Lua...\n");
#endif

	L = lua_open();

	if(!L)
	{
#if defined (CLIENT_DLL)
		Warning("[Lua] Unable to initialize client-side Lua.\n");
#else
		Warning("[Lua] Unable to initialize server-side Lua.\n");
#endif
		return;
	}
	
	// load all libraries
	luaL_openlibs(L);
	
	lua_register(L,"print",print);

	// load base libraries
	//luaopen_base(L);
	//luaopen_table(L);
	//luaopen_string(L);
	//luaopen_math(L);

	// initialize luabind
	luabind::open(L);
	
}

void lua_shutdown()
{
	if(L)
	{
		lua_close(L);
		L = NULL;
	}
}

int luasrc_dostring (lua_State *L, const char *string) {
  int iError = luaL_dostring(L, string);
  if (iError != 0) {
    Warning( "%s\n", lua_tostring(L, -1) );
    lua_pop(L, 1);
  }
  return iError;
}

#if defined( GAME_DLL )
CON_COMMAND( lua_dostring_sv, "Run a server-side Lua string" )
#else
CON_COMMAND( lua_dostring_cl, "Run a client-side Lua string" )
#endif
{
	if ( args.ArgC() == 1 )
	{
		Msg( "Usage: lua_dostring <string>\n" );
		return;
	}

	int status = luasrc_dostring( L, args.ArgS() );
	if (status == 0 && lua_gettop(L) > 0) {  /* any result to print? */
		lua_getglobal(L, "print");
		lua_insert(L, 1);
		if (lua_pcall(L, lua_gettop(L)-1, 0, 0) != 0)
		Warning("%s", lua_pushfstring(L,
							"error calling " LUA_QL("print") " (%s)",
							lua_tostring(L, -1)));
	}
	lua_settop(L, 0);  /* clear stack */
}