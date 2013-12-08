
/////////////////////////////////////////////////////////////////////////////
// includes
#include "cbase.h"
#include "ff_cl_scriptman_ui.h"

// engine
//#include "filesystem.h"

// dexter note 10/29/2013 these are definitely still needed for lua/luabind
#undef MINMAX_H
#undef min
#undef max
//
// luabind
#include "lua.hpp"
#include "luabind/luabind.hpp"
//#include "luabind/object.hpp"
//#include "luabind/iterator_policy.hpp"

// binding
#include "ff_sh_luabind.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace luabind;

CFF_CL_ScriptManager_UI g_UIScriptManager;

/////////////////////////////////////////////////////////////////////////////
CFF_CL_ScriptManager_UI::CFF_CL_ScriptManager_UI()
{
}

/////////////////////////////////////////////////////////////////////////////
CFF_CL_ScriptManager_UI::~CFF_CL_ScriptManager_UI()
{
}

void CFF_CL_ScriptManager_UI::SetupEnvironmentForFF()
{
	BaseClass::SetupEnvironmentForFF();

	FF_Lua_InitUI( L );
}

/////////////////////////////////////////////////////////////////////////////
void CFF_CL_ScriptManager_UI::Shutdown()
{
	BaseClass::Shutdown();
}


/////////////////////////////////////////////////////////////////////////////
bool CFF_CL_ScriptManager_UI::Init()
{
	bool bInitSuccessful = BaseClass::Init();

	if (bInitSuccessful)
	{
		LoadFile( "ui/init.lua" );

		// load layout settings
		luabind::object luaobjLayoutSettings;
		LoadFileIntoObject( "ui/settings/layout.lua", luaobjLayoutSettings );

		// set global variable LAYOUT to the loaded settings
		(luabind::globals(L))["LAYOUT"] = luaobjLayoutSettings;
		
		// load theme settings
		luabind::object luaobjThemeSettings;
		LoadFileIntoObject( "ui/settings/theme_default.lua", luaobjThemeSettings );
		LoadFileIntoObject( "ui/settings/theme.lua", luaobjThemeSettings ); // custom theme settings
		
		// set global variable THEME to the loaded settings
		(luabind::globals(L))["THEME"] = luaobjThemeSettings;
	}

	return bInitSuccessful;
}

/////////////////////////////////////////////////////////////////////////////
void CFF_CL_ScriptManager_UI::LevelInit(const char* szMapName)
{

}

/////////////////////////////////////////////////////////////////////////////
void CFF_CL_ScriptManager_UI::LevelShutdown()
{

}


CON_COMMAND( lua_dostring_ui, "Run a client-side Lua string in the UI environment" )
{
	if ( args.ArgC() == 1 )
	{
		Msg( "Usage: lua_dostring <string>\n" );
		return;
	}

	lua_State *L = g_UIScriptManager.GetLuaState();
	int status = luaL_dostring(L, args.ArgS());
	if (status != 0) {
		Warning( "%s\n", lua_tostring(L, -1) );
		lua_pop(L, 1);
		return;
	}
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