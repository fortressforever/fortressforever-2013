
/////////////////////////////////////////////////////////////////////////////
// includes
#include "cbase.h"
#include "ff_sh_scriptman.h"
//#include "ff_entity_system.h"
//#include "ff_luacontext.h"
//#include "ff_lualib.h"
//#include "ff_utils.h"
//#include "ff_item_flag.h"
//#include "triggers.h"

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

#ifdef CLIENT_DLL
#define LUA_CURRENT_CONTEXT "Client"
#else
#define LUA_CURRENT_CONTEXT "Server"
#endif

// custom game modes made so damn easy
ConVar sv_mapluasuffix( "sv_mapluasuffix", "0", FCVAR_NOTIFY, "Have a custom lua file (game mode) loaded when the map loads. If this suffix string is set, maps\\mapname__suffix__.lua (if it exists) is used instead of maps\\mapname.lua. To reset this cvar, make it 0.");
ConVar sv_luaglobalscript( "sv_globalluascript", "0", FCVAR_NOTIFY, "Load a custom lua file globally after map scripts. Will overwrite map script. Will be loaded from maps\\globalscripts. To disable, set to 0.");

/////////////////////////////////////////////////////////////////////////////
using namespace luabind;

/////////////////////////////////////////////////////////////////////////////
// globals
CFF_SH_ScriptManager g_GameScriptManager;
#ifdef CLIENT_DLL
CFF_SH_ScriptManager g_UIScriptManager;
#endif


/////////////////////////////////////////////////////////////////////////////
CFF_SH_ScriptManager::CFF_SH_ScriptManager()
: L(NULL)
, m_isLoading(false)
, m_scriptCRC(0)
, m_ScriptExists(false)
{
}

/////////////////////////////////////////////////////////////////////////////
CFF_SH_ScriptManager::~CFF_SH_ScriptManager()
{
	Shutdown();
}


/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager::Shutdown()
{
	m_ScriptExists = false;

	// shtutdown VM
	if(L)
	{
		lua_close(L);
		L = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager::Init()
{
	// shutdown VM if already running
	if(L)
	{
		lua_close(L);
		L = NULL;
	}

	// initialize VM
	Msg("[SCRIPT:%s] Attempting to start the Lua VM...\n", LUA_CURRENT_CONTEXT);
	L = lua_open();

	// no need to continue if VM failed to initialize
	if(!L)
	{
		Msg("[SCRIPT:%s] Unable to initialize Lua VM.\n", LUA_CURRENT_CONTEXT);
		return;
	}

	// load base libraries
	luaopen_base(L);
	luaopen_table(L);
	luaopen_string(L);
	luaopen_math(L);

	// initialize luabind
	luabind::open(L);

	// initialize game-specific library
	//FF_TODO: CFFLuaLib::Init(L); 
	
	Msg("[SCRIPT:%s] Lua VM initialization successful.\n", LUA_CURRENT_CONTEXT);
}

void CFF_SH_ScriptManager::LevelInit(const char* szMapName)
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
	LoadFile(L, "maps/includes/base.lua");

	char filename[256] = {0};
	char globalscript_filename[256] = {0};
	// Even though LoadFile already checks to see if the file exists, we'll check now so at least the default map lua file is loaded.
	// That way servers can keep their suffix set without worrying about every map having whatever game mode they always want to use.
	if ( sv_mapluasuffix.GetString()[0] != '0' )
	{
		Msg( "[SCRIPT] sv_mapluasuffix set to %s | finding maps\\%s__%s__.lua\n", sv_mapluasuffix.GetString(), szMapName, sv_mapluasuffix.GetString() );
#ifdef CLIENT_DLL
		if ( filesystem->FileExists( VarArgs( "maps/%s__%s__.lua", szMapName, sv_mapluasuffix.GetString() ) ) )
#else
		if ( filesystem->FileExists( UTIL_VarArgs( "maps/%s__%s__.lua", szMapName, sv_mapluasuffix.GetString() ) ) )
#endif
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
#ifdef CLIENT_DLL
		if( filesystem->FileExists( VarArgs( "maps/globalscripts/%s.lua", scriptname ) ) )
#else
		if( filesystem->FileExists( UTIL_VarArgs( "maps/globalscripts/%s.lua", scriptname ) ) )
#endif
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
	//////////////////////////////////////////////////////////////////////////
	// Try a precache, rumor has it this will cause the engine to send the lua files to clients
	// FF_TODO: dexter - disabled for now, doesnt work anyway right?
	//if(PRECACHE_LUA_FILES)
	//{
	//	V_FixSlashes(filename);
	//	if(filesystem->FileExists(filename))
	//	{
	//		Util_AddDownload(filename);

	//		if(!engine->IsGenericPrecached(filename))
	//			engine->PrecacheGeneric(filename, true);
	//	} 
	//	else // - if no map lua is found, send default (for testing mainly)
	//	{
	//		// no check - this file should *always* be there
	//		Util_AddDownload(default_luafile);
	//		if(!engine->IsGenericPrecached(default_luafile))
	//			engine->PrecacheGeneric(default_luafile, true);
	//	}
	//	

	//	// if we have a globalscript, precache it as well
	//	if( sv_luaglobalscript.GetString()[0] != '0' && globalscript_filename[0] )
	//	{
	//		V_FixSlashes(globalscript_filename);
	//		if(filesystem->FileExists(globalscript_filename))
	//		{
	//			Util_AddDownload(globalscript_filename);

	//			if(!engine->IsGenericPrecached(globalscript_filename))
	//				engine->PrecacheGeneric(globalscript_filename, true);
	//		}
	//	}

	//	//////////////////////////////////////////////////////////////////////////
	//	/*char testfile[] = {"maps/ff_dm.txt"};
	//	V_FixSlashes(testfile);
	//	if(filesystem->FileExists(testfile))
	//	{
	//		Util_AddDownload(testfile);

	//		if(!engine->IsGenericPrecached(testfile))
	//			engine->PrecacheGeneric(testfile, true);
	//	}*/
	//	//////////////////////////////////////////////////////////////////////////
	//}
	//////////////////////////////////////////////////////////////////////////
	if(filesystem->FileExists(filename))
		m_ScriptExists = LoadFile(L, filename);
	else
	{
		Msg("[SCRIPT] File %s not found! Loaded fallback lua %s\n", filename, default_luafile);
		m_ScriptExists = LoadFile(L, default_luafile);
	}

	// force loading global script in another call :/
	if( sv_luaglobalscript.GetString()[0] != '0' && globalscript_filename[0] )
	{
		//BeginScriptLoad();
		LoadFile(L, globalscript_filename);
		//EndScriptLoad();
	}

	EndScriptLoad();

	// spawn the helper entity
	//FF_TODO: CFFEntitySystemHelper::Create();
}

/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager::LevelShutdown()
{
	// shutdown the VM
	if(L)
	{
		lua_close(L);
		L = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager::OnScriptLoad(const char* szFileName,
								   const char* szFileContents)
{
	// ignore the message if we are not still in the "loading" phase
	if(!m_isLoading)
		return;

	// compute checksums of file contents
	CRC32_ProcessBuffer(&m_scriptCRC,
						szFileContents,
						strlen(szFileContents));
}

/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager::BeginScriptLoad()
{
	CRC32_Init(&m_scriptCRC);
	m_isLoading = true;
}

/////////////////////////////////////////////////////////////////////////////
void CFF_SH_ScriptManager::EndScriptLoad()
{
	CRC32_Final(&m_scriptCRC);
	m_isLoading = false;
}


/////////////////////////////////////////////////////////////////////////////
bool CFF_SH_ScriptManager::LoadFile( lua_State *L, const char *filename)
{
	//FF_TODO: VPROF_BUDGET( "CFF_SH_ScriptManager::LoadFile", VPROF_BUDGETGROUP_FF_LUA );

	// don't allow scripters to sneak in scripts after the initial load
	if(!m_isLoading)
	{
		Warning("[SCRIPT] Loading of scripts after initial map load is not allowed.\n");
		return false;
	}

	// open the file
	Msg("[SCRIPT] Loading Lua File: %s\n", filename);
	FileHandle_t hFile = filesystem->Open(filename, "rb", "MOD");

	if (!hFile)
	{
		Warning("[SCRIPT] %s either does not exist or could not be opened.\n", filename);
		return false;
	}

	// allocate buffer for file contents
	int fileSize = filesystem->Size(hFile);
	char *buffer = (char*)MemAllocScratch(fileSize + 1);
	Assert(buffer);
		
	// load file contents into a null-terminated buffer
	filesystem->Read(buffer, fileSize, hFile);
	buffer[fileSize] = 0;
	filesystem->Close(hFile);

	// preprocess script data
	OnScriptLoad(filename, buffer);

	// load script
	int errorCode = luaL_loadbuffer(L, buffer, fileSize, filename);

	// check if load was successful
	if(errorCode)
	{
		if(errorCode == LUA_ERRSYNTAX )
		{
			// syntax error, lookup description for the error
			const char *error = lua_tostring(L, -1);
			if (error)
			{
				Warning("Error loading %s: %s\n", filename, error);
				lua_pop( L, 1 );
			}
			else
				Warning("Unknown Syntax Error loading %s\n", filename);
		}
		else
		{
			Msg("Unknown Error loading %s\n", filename);
		}
		return false;
	}

	// execute script. script at top scrop gets exectued
	lua_pcall(L, 0, 0, 0);
	Msg( "[SCRIPT] Successfully loaded %s\n", filename );

	// cleanup
	MemFreeScratch();
	return true;
}