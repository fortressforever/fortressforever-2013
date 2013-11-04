#ifndef FF_SH_SCRIPTMAN_H
#define FF_SH_SCRIPTMAN_H
#pragma once

/////////////////////////////////////////////////////////////////////////////
// includes
#ifndef CHECKSUM_CRC_H
	#include "checksum_crc.h"
#endif

#define PRECACHE_LUA_FILES 1

/////////////////////////////////////////////////////////////////////////////
// forward declarations
struct lua_State;

namespace luabind
{
	namespace adl
	{
		class object;
	}
}

class CFFLuaSC;

/////////////////////////////////////////////////////////////////////////////
class CFF_SH_ScriptManager
{
public:
	DECLARE_CLASS_NOBASE( CFF_SH_ScriptManager );
	// 'structors
	CFF_SH_ScriptManager();
	~CFF_SH_ScriptManager();

	// inserts the lua file into the script environment
	virtual bool LoadFile(const char* filePath);
	// removes unsafe functions from the Lua environment or makes them safer
	virtual void MakeEnvironmentSafe();
	virtual void SetupEnvironmentForFF();

	// initializes the script VM
	virtual bool Init();
	// closes the script VM
	virtual void Shutdown();
	
	virtual void LevelInit(const char* szMapName) {};
	virtual void LevelShutdown() {};

	void LuaMsg( const tchar* pMsg, ... ) FMTFUNCTION( 1, 2 );
	void LuaWarning( const tchar* pMsg, ... ) FMTFUNCTION( 1, 2 );

protected:
	virtual const char* GetMsgIdentifier() { return "Generic"; };
	virtual const char* GetPackagePathRoot() { return "/"; };
	
	/// Called from LoadFile
	virtual void OnScriptLoad(const char* szFileName, const char* szFileContents) {};

/*
public:
	// sets a global variable in the script environment
	static void SetVar( lua_State *L, const char *name, const char *value );
	static void SetVar( lua_State *L, const char *name, int value );
	static void SetVar( lua_State *L, const char *name, float value );
	void SetVar(const char* name, const char* value);
	void SetVar(const char* name, int value);
	void SetVar(const char* name, float value);

	// gets a global variable from the script environment
	const char* GetString(const char* name);
	int GetInt(const char* name);
	float GetFloat(const char* name);

	bool GetObject(CBaseEntity* pEntity, luabind::adl::object& outObject);
	bool GetObject(const char* szTableName, luabind::adl::object& outObject);

	bool GetFunction(CBaseEntity* pEntity,
					 const char* szFunctionName,
					 luabind::adl::object& outObject);

	bool GetFunction(luabind::adl::object& tableObject,
					 const char* szFunctionName,
					 luabind::adl::object& outObject);

public:
	int RunPredicates( CBaseEntity *pObject, CBaseEntity *pEntity, const char *szFunctionName = NULL);
	bool RunPredicates_LUA( CBaseEntity *pObject, CFFLuaSC *pContext, const char *szFunctionName );
*/

public:
	// returns the lua interpreter
	lua_State* GetLuaState() const { return L; }

protected:
	lua_State*	L;					///< Lua VM
};

/////////////////////////////////////////////////////////////////////////////
#endif
