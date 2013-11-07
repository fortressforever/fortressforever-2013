#pragma once
#ifndef FF_CL_SCRIPTMAN_UI_H
#define FF_CL_SCRIPTMAN_UI_H

#include "ff_sh_scriptman.h"

class CFF_CL_ScriptManager_UI : public CFF_SH_ScriptManager
{
public:
	DECLARE_CLASS( CFF_CL_ScriptManager_UI, CFF_SH_ScriptManager );
	// 'structors
	CFF_CL_ScriptManager_UI();
	~CFF_CL_ScriptManager_UI();

protected:
	virtual const char* GetMsgIdentifier() { return "UI"; };
	virtual const char* GetPackagePathRoot() { return "/ui/"; };

public:
	virtual void SetupEnvironmentForFF();

	virtual bool Init();
	virtual void Shutdown();

	virtual void LevelInit(const char* szMapName);
	virtual void LevelShutdown();

};

extern CFF_CL_ScriptManager_UI g_UIScriptManager;

#endif