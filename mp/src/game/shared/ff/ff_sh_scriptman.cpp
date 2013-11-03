
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
#include "tier1/strtools.h"

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

// redirect Lua's print function to the console
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
	
	// load all libraries
	luaL_openlibs(L);
	// make the standard libraries safe
	MakeSafe();
	
	// overwrite Lua's print with our own
	lua_register(L,"print",print);
	
	// set package.path to the mod search dirs, so that we can use require
	char szModSearchPaths[4096] = {0};
	// FF TODO: This is set to ignore pack files; might need to add support for them (for custom mods packaged as .vpks)
	filesystem->GetSearchPath( "MOD", false, szModSearchPaths, sizeof(szModSearchPaths) );
	
	char szLuaSearchPaths[4096] = {0};
	for ( char *szSearchPath = strtok( szModSearchPaths, ";" ); szSearchPath; szSearchPath = strtok( NULL, ";" ) )
	{
		char fullpath[MAX_PATH];
		Q_snprintf( fullpath, sizeof( fullpath ), "%s%s", szSearchPath, "/?.lua;" );
		Q_FixSlashes( fullpath );
		V_FixDoubleSlashes( fullpath );

		Q_strncat( szLuaSearchPaths, fullpath, sizeof(szLuaSearchPaths) );
	}
	
	// set package.path; this is equivelent to the Lua code: _G.package["path"] = szLuaSearchPaths
	lua_pushstring(L, "package");
	lua_gettable(L, LUA_GLOBALSINDEX); // get _G.package
	lua_pushstring(L, "path");
	lua_pushstring(L, szLuaSearchPaths);
	lua_settable(L, -3); // -3 is the package table
	lua_pop(L, 1); // pop _G.package

	// initialize luabind
	luabind::open(L);

	// initialize game-specific library
	//FF_TODO: CFFLuaLib::Init(L); 
	
	Msg("[SCRIPT:%s] Lua VM initialization successful.\n", LUA_CURRENT_CONTEXT);
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

/** Get rid of or alter any unsafe Lua functions
*/
void CFF_SH_ScriptManager::MakeSafe()
{
	// os.*
	const char* ppszUnsafeOSFunctions[] = { "execute", "exit", "getenv", "remove", "rename", "setlocale", NULL };
	LUAUTIL_RemoveKeysFromTable( L, LUA_OSLIBNAME, ppszUnsafeOSFunctions );
	
	// package.*
	const char* ppszUnsafePackageFunctions[] = { "loadlib", NULL };
	LUAUTIL_RemoveKeysFromTable( L, LUA_LOADLIBNAME, ppszUnsafePackageFunctions );
	
	// require can load .dll/.so, need to disable the loaders that search for them
	// the third index checks package.cpath and loads .so/.dll files
	// the fourth index is an all-in-one loader that can load .so/.dll files
	lua_getglobal(L, LUA_LOADLIBNAME);
	lua_pushstring(L, "loaders");
	lua_gettable(L, -2);
	lua_pushnil(L);
	lua_rawseti(L, -2, 4); // _G.package.loaders[4] = nil
	lua_pushnil(L);
	lua_rawseti(L, -2, 3); // _G.package.loaders[3] = nil
	lua_pop(L, 2); // pop _G.package.loaders and _G.package

	// FF TODO: restrict io library, maybe disable it completely
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
	LoadFile("maps/includes/base.lua");

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
		m_ScriptExists = LoadFile(filename);
	else
	{
		Msg("[SCRIPT] File %s not found! Loaded fallback lua %s\n", filename, default_luafile);
		m_ScriptExists = LoadFile(default_luafile);
	}

	// force loading global script in another call :/
	if( sv_luaglobalscript.GetString()[0] != '0' && globalscript_filename[0] )
	{
		//BeginScriptLoad();
		LoadFile(globalscript_filename);
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
bool CFF_SH_ScriptManager::LoadFile(const char *filename)
{
	//FF_TODO: VPROF_BUDGET( "CFF_SH_ScriptManager::LoadFile", VPROF_BUDGETGROUP_FF_LUA );

	// open the file
	Msg("[SCRIPT:%s] Loading Lua File: %s\n", LUA_CURRENT_CONTEXT, filename);
	FileHandle_t hFile = filesystem->Open(filename, "rb", "MOD");

	if (!hFile)
	{
		Warning("[SCRIPT:%s] %s either does not exist or could not be opened.\n", LUA_CURRENT_CONTEXT, filename);
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
	if (errorCode != 0)
	{
		const char *error = lua_tostring(L, -1);
		Warning( "[SCRIPT:%s] Error loading %s: %s\n", LUA_CURRENT_CONTEXT, filename, error );
		lua_pop( L, 1 );
		return false;
	}

	// execute script. script at top scrop gets exectued
	errorCode = lua_pcall(L, 0, 0, 0);
	
	// check if execution was successful
	if (errorCode != 0)
	{
		const char *error = lua_tostring(L, -1);
		Warning( "[SCRIPT:%s] Error loading %s: %s\n", LUA_CURRENT_CONTEXT, filename, error );
		lua_pop( L, 1 );
		return false;
	}

	Msg( "[SCRIPT:%s] Successfully loaded %s\n", LUA_CURRENT_CONTEXT, filename );

	// cleanup
	MemFreeScratch();
	return true;
}

int luasrc_dostring (lua_State *L, const char *string) {
  int iError = luaL_dostring(L, string);
  if (iError != 0) {
    Warning( "%s\n", lua_tostring(L, -1) );
    lua_pop(L, 1);
  }
  return iError;
}

#ifdef CLIENT_DLL
CON_COMMAND( lua_dostring_ui, "Run a client-side Lua string in the UI environment" )
{
	if ( args.ArgC() == 1 )
	{
		Msg( "Usage: lua_dostring <string>\n" );
		return;
	}

	lua_State *L = g_UIScriptManager.GetLuaState();
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
#endif