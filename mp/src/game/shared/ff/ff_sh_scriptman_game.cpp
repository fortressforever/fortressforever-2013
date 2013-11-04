
/////////////////////////////////////////////////////////////////////////////
// includes
#include "cbase.h"
#include "ff_sh_scriptman_game.h"

// engine
#include "filesystem.h"

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

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// custom game modes made so damn easy
ConVar sv_mapluasuffix( "sv_mapluasuffix", "0", FCVAR_NOTIFY, "Have a custom lua file (game mode) loaded when the map loads. If this suffix string is set, maps\\mapname__suffix__.lua (if it exists) is used instead of maps\\mapname.lua. To reset this cvar, make it 0.");
ConVar sv_luaglobalscript( "sv_globalluascript", "0", FCVAR_NOTIFY, "Load a custom lua file globally after map scripts. Will overwrite map script. Will be loaded from maps\\globalscripts. To disable, set to 0.");

using namespace luabind;

CFF_SH_ScriptManager_Game g_GameScriptManager;

/////////////////////////////////////////////////////////////////////////////
CFF_SH_ScriptManager_Game::CFF_SH_ScriptManager_Game()
{
	m_bLoading = m_bScriptLoaded = false;
	m_ScriptCRC = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CFF_SH_ScriptManager_Game::~CFF_SH_ScriptManager_Game()
{
}


/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager_Game::Shutdown()
{
	BaseClass::Shutdown();
}


/////////////////////////////////////////////////////////////////////////////
bool CFF_SH_ScriptManager_Game::Init()
{
	bool bInitSuccessful = BaseClass::Init();

	if (bInitSuccessful)
	{

	}

	return bInitSuccessful;
}

void CFF_SH_ScriptManager_Game::LevelInit(const char* szMapName)
{
	const char* default_luafile = "maps/default.lua";
	//FF_TODO: VPROF_BUDGET("CFF_SH_ScriptManager::LevelInit", VPROF_BUDGETGROUP_FF_LUA);

	if(!szMapName)
		return;

	//FF_TODO: g_Disable_Timelimit = false;

	// setup VM
	Init();
	
	// load lua files
	BeginScriptLoad();
	LoadFile("maps/includes/base.lua");

	char filename[256] = {0};
	char globalscript_filename[256] = {0};
	// Even though LoadFile already checks to see if the file exists, we'll check now so at least the default map lua file is loaded.
	// That way servers can keep their suffix set without worrying about every map having whatever game mode they always want to use.
	if ( sv_mapluasuffix.GetString()[0] != '0' )
	{
		Msg( "[SCRIPT] sv_mapluasuffix set to %s | finding maps\\%s__%s__.lua\n", sv_mapluasuffix.GetString(), szMapName, sv_mapluasuffix.GetString() );
		if ( filesystem->FileExists( UTIL_VarArgs( "maps/%s__%s__.lua", szMapName, sv_mapluasuffix.GetString() ) ) )
		{
			Q_snprintf( filename, sizeof(filename), "maps/%s__%s__.lua", szMapName, sv_mapluasuffix.GetString() );
			Msg( "[SCRIPT] maps\\%s__%s__.lua found\n", szMapName, sv_mapluasuffix.GetString() );
		}
		else
		{
			Msg( "[SCRIPT] maps\\%s__%s__.lua not found | reverting to maps\\%s.lua\n", szMapName, sv_mapluasuffix.GetString(), szMapName);
		}
	}

	// Load global include script, overwriting previously loaded stuff per map
	if( sv_luaglobalscript.GetString()[0] != '0' )
	{
		const char* scriptname = sv_luaglobalscript.GetString();
		Msg("[SCRIPT] sv_luaglobalscript set to %s | loading global script maps maps\\globalscripts\\%s.lua\n", scriptname, scriptname );
		if( filesystem->FileExists( UTIL_VarArgs( "maps/globalscripts/%s.lua", scriptname ) ) )
		{
			Q_snprintf( globalscript_filename, sizeof(globalscript_filename), "maps/globalscripts/%s.lua", scriptname );
			Msg("[SCRIPT] maps\\globalscripts\\%s.lua found\n", scriptname );\
		}
		else
		{
			Msg("[SCRIPT] global script maps\\globalscripts\\%s.lua not found - nothing loaded post map lua.\n", scriptname );
		}
	}

	if ( !filename[0] )
		Q_snprintf( filename, sizeof(filename), "maps/%s.lua", szMapName );

	if(filesystem->FileExists(filename))
		m_bScriptLoaded = LoadFile(filename);
	else
	{
		Msg("[SCRIPT] File %s not found! Loaded fallback lua %s\n", filename, default_luafile);
		m_bScriptLoaded = LoadFile(default_luafile);
	}

	// force loading global script in another call :/
	if( sv_luaglobalscript.GetString()[0] != '0' && globalscript_filename[0] )
	{
		LoadFile(globalscript_filename);
	}

	EndScriptLoad();

	// spawn the helper entity
	//FF_TODO: CFFEntitySystemHelper::Create();
}

/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager_Game::LevelShutdown()
{
	// shutdown the VM
	if(L)
	{
		lua_close(L);
		L = NULL;
	}
}

void CFF_SH_ScriptManager_Game::OnScriptLoad(const char* szFileName, const char* szFileContents)
{
	// ignore the message if we are not still in the "loading" phase
	if(!m_bLoading)
		return;

	// compute checksums of file contents
	CRC32_ProcessBuffer( &m_ScriptCRC, szFileContents, strlen(szFileContents) );
}

void CFF_SH_ScriptManager_Game::BeginScriptLoad()
{
	CRC32_Init(&m_ScriptCRC);
	m_bLoading = true;
}

void CFF_SH_ScriptManager_Game::EndScriptLoad()
{
	CRC32_Final(&m_ScriptCRC);
	m_bLoading = false;
}

#ifdef CLIENT_DLL
CON_COMMAND( lua_dostring_game_cl, "Run a client-side Lua string in the Game environment" )
#else
CON_COMMAND( lua_dostring_game_sv, "Run a server-side Lua string in the Game environment" )
#endif
{
	if ( args.ArgC() == 1 )
	{
		Msg( "Usage: lua_dostring <string>\n" );
		return;
	}

	lua_State *L = g_GameScriptManager.GetLuaState();
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