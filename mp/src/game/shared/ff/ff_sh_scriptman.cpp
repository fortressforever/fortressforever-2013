
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
#include "ff_sh_luautil.h"

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

using namespace luabind;

CFF_SH_ScriptManager::CFF_SH_ScriptManager()
{
	L = NULL;
}

CFF_SH_ScriptManager::~CFF_SH_ScriptManager()
{
	Shutdown();
}

/** Close the Lua VM
*/
void CFF_SH_ScriptManager::Shutdown()
{
	// shtutdown VM
	if(L)
	{
		lua_close(L);
		L = NULL;
	}
}

/** Open the Lua VM
	@returns True if successful, false if couldn't open Lua VM
*/
bool CFF_SH_ScriptManager::Init()
{
	// shutdown VM if already running
	Shutdown();

	// initialize VM
	LuaMsg("Attempting to start the Lua VM...\n");
	L = lua_open();

	// no need to continue if VM failed to initialize
	if(!L)
	{
		LuaMsg("Unable to initialize Lua VM.\n");
		return false;
	}
	
	// initialize all the FF specific stuff
	SetupEnvironmentForFF();

	// make the standard libraries safe
	MakeEnvironmentSafe();

	LuaMsg("Lua VM initialization successful.\n");
	return true;
}

/** Loads the Lua libraries and sets all the variables needed for FF
*/
void CFF_SH_ScriptManager::SetupEnvironmentForFF()
{
	Assert(L);
	if (!L) return;

	// load all libraries
	luaL_openlibs(L);

	// overwrite Lua's print with our own
	lua_register(L,"print",print);
	
	// set package.path to the mod search dirs, so that we can use require
	char szModSearchPaths[4096] = {0};
	// FF TODO: This is set to ignore pack files; might need to add support for them (for custom mods packaged as .vpks?)
	filesystem->GetSearchPath( "MOD", false, szModSearchPaths, sizeof(szModSearchPaths) );
	
	char szLuaSearchPaths[4096] = {0};
	for ( char *szSearchPath = strtok( szModSearchPaths, ";" ); szSearchPath; szSearchPath = strtok( NULL, ";" ) )
	{
		char fullpath[MAX_PATH];
		Q_snprintf( fullpath, sizeof( fullpath ), "%s%s%s", szSearchPath, GetPackagePathRoot(), "?.lua;" );
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
}

/** Gets rid of or alter any unsafe Lua functions in the current environment
*/
void CFF_SH_ScriptManager::MakeEnvironmentSafe()
{
	Assert(L);
	if (!L) return;

	// See: http://lua-users.org/wiki/SandBoxes for a general overview of the safety of Lua functions

	// dofile, load, loadfile, loadstring
	const char* ppszUnsafeGlobalFunctions[] = { "dofile", "load", "loadfile", "loadstring", NULL };
	LUAUTIL_RemoveVarsFromGlobal( L, ppszUnsafeGlobalFunctions );

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
	lua_gettable(L, -2); // get _G.package.loaders
	lua_pushnil(L);
	lua_rawseti(L, -2, 4); // _G.package.loaders[4] = nil
	lua_pushnil(L);
	lua_rawseti(L, -2, 3); // _G.package.loaders[3] = nil
	lua_pop(L, 2); // pop _G.package.loaders and _G.package

	// FF TODO: restrict io library, maybe disable it completely
	// FF TODO: somehow protect package.path? not sure if it's necessary, the "risk" would be allowing execution of .lua files in arbitrary locations
}

/** Loads a Lua file into the current environment relative to a "MOD" search path
	@returns True if file successfully loaded, false if there were any errors (syntax or execution)
*/
bool CFF_SH_ScriptManager::LoadFile(const char *filename)
{
	//FF_TODO: VPROF_BUDGET( "CFF_SH_ScriptManager::LoadFile", VPROF_BUDGETGROUP_FF_LUA );

	// open the file
	LuaMsg("Loading Lua File: %s\n", filename);
	FileHandle_t hFile = filesystem->Open(filename, "rb", "MOD");

	if (!hFile)
	{
		LuaWarning("%s either does not exist or could not be opened.\n", filename);
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
		LuaWarning( "Error loading %s: %s\n", filename, error );
		lua_pop( L, 1 );
		return false;
	}

	// execute script. script at top scrop gets exectued
	errorCode = lua_pcall(L, 0, 0, 0);
	
	// check if execution was successful
	if (errorCode != 0)
	{
		const char *error = lua_tostring(L, -1);
		LuaWarning( "Error loading %s: %s\n", filename, error );
		lua_pop( L, 1 );
		return false;
	}

	LuaMsg( "Successfully loaded %s\n", filename );

	// cleanup
	MemFreeScratch();
	return true;
}

// FF TODO: Better way to get this sort of string?
#ifdef CLIENT_DLL
#define LUA_MSG_DLL_CONTEXT "Client"
#else
#define LUA_MSG_DLL_CONTEXT "Server"
#endif

/** Wrapper for Msg that prefixes the string with info about where it's coming from in the format: [Lua.<GetMsgIdentifier()>:<Client/Server>]
*/
void CFF_SH_ScriptManager::LuaMsg( const char *pszFormat, ... )
{
	va_list		argptr;
	static char		string[4096];
	
	va_start (argptr, pszFormat);
	Q_vsnprintf(string, sizeof(string), pszFormat, argptr);
	va_end (argptr);

	Msg("[Lua.%s:%s] %s", GetMsgIdentifier(), LUA_MSG_DLL_CONTEXT, string );
}

/** Wrapper for Warning that prefixes the string with info about where it's coming from in the format: [Lua.<GetMsgIdentifier()>:<Client/Server>]
*/
void CFF_SH_ScriptManager::LuaWarning( const char *pszFormat, ... )
{
	va_list		argptr;
	static char		string[4096];
	
	va_start (argptr, pszFormat);
	Q_vsnprintf(string, sizeof(string), pszFormat, argptr);
	va_end (argptr);

	Warning("[Lua.%s:%s] %s", GetMsgIdentifier(), LUA_MSG_DLL_CONTEXT, string );
}