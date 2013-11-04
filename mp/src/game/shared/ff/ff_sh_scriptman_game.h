#pragma once
#ifndef FF_CL_SCRIPTMAN_GAME_H
#define FF_CL_SCRIPTMAN_GAME_H

#include "ff_sh_scriptman.h"

class CFF_SH_ScriptManager_Game : public CFF_SH_ScriptManager
{
public:
	DECLARE_CLASS( CFF_SH_ScriptManager_Game, CFF_SH_ScriptManager );
	// 'structors
	CFF_SH_ScriptManager_Game();
	~CFF_SH_ScriptManager_Game();

protected:
	virtual const char* GetMsgIdentifier() { return "Game"; };
	virtual const char* GetPackagePathRoot() { return "/lua/"; };
	
	virtual void OnScriptLoad(const char* szFileName, const char* szFileContents);

public:
	virtual bool Init();
	virtual void Shutdown();

	virtual void LevelInit(const char* szMapName);
	virtual void LevelShutdown();
	
	bool ScriptLoaded( void ) const { return m_bScriptLoaded; }
	CRC32_t GetScriptCRC() const { return m_ScriptCRC; }

private:
	// surround code that loads scripts to capture crc checksum
	// of the scripts that are loaded
	void BeginScriptLoad();
	void EndScriptLoad();
	
private:
	bool		m_bScriptLoaded;	///< Whether or not a script file has been loaded in the current Lua environment
	bool		m_bLoading;			///< True at the start of LevelInit, false at the end
	CRC32_t		m_ScriptCRC;		///< CRC of all scripts loaded while m_isLoading was true

};

extern CFF_SH_ScriptManager_Game g_GameScriptManager;

#endif