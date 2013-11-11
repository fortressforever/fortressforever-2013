//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//
#include "cbase.h"
#include "hud.h"
#include "ff_cl_clientmode.h"
#include "cdll_client_int.h"
#include "iinput.h"
#include "vgui/ISurface.h"
#include "vgui/IPanel.h"
#include <vgui_controls/AnimationController.h>
#include "ivmodemanager.h"
#include "buymenu.h"
#include "filesystem.h"
#include "vgui/IVGui.h"
#include "hud_chat.h"
#include "view_shared.h"
#include "view.h"
#include "ivrenderview.h"
#include "model_types.h"
#include "iefx.h"
#include "dlight.h"
#include <imapoverview.h>
#include "c_playerresource.h"
#include <KeyValues.h>
#include "text_message.h"
#include "panelmetaclassmgr.h"


ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );

IClientMode *g_pClientMode = NULL;


// --------------------------------------------------------------------------------- //
// CFF_CL_ClientModeManager.
// --------------------------------------------------------------------------------- //

class CFF_CL_ClientModeManager : public IVModeManager
{
public:
	virtual void	Init();
	virtual void	SwitchMode( bool commander, bool force ) {}
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
	virtual void	ActivateMouse( bool isactive ) {}
};

static CFF_CL_ClientModeManager g_ModeManager;
IVModeManager *modemanager = ( IVModeManager * )&g_ModeManager;

// --------------------------------------------------------------------------------- //
// CFF_CL_ClientModeManager implementation.
// --------------------------------------------------------------------------------- //

#define SCREEN_FILE		"scripts/vgui_screens.txt"

void CFF_CL_ClientModeManager::Init()
{
	g_pClientMode = GetClientModeNormal();
	
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CFF_CL_ClientModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );
}

void CFF_CL_ClientModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFF_CL_ClientModeNormal::CFF_CL_ClientModeNormal()
{
}

//-----------------------------------------------------------------------------
// Purpose: If you don't know what a destructor is by now, you are probably going to get fired
//-----------------------------------------------------------------------------
CFF_CL_ClientModeNormal::~CFF_CL_ClientModeNormal()
{
}


void CFF_CL_ClientModeNormal::InitViewport()
{
	m_pViewport = new CFF_CL_HudViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}

CFF_CL_ClientModeNormal g_ClientModeNormal;

IClientMode *GetClientModeNormal()
{
	return &g_ClientModeNormal;
}

CFF_CL_ClientModeNormal* GetClientModeFFNormal()
{
	Assert( dynamic_cast< CFF_CL_ClientModeNormal* >( GetClientModeNormal() ) );

	return static_cast< CFF_CL_ClientModeNormal* >( GetClientModeNormal() );
}

float CFF_CL_ClientModeNormal::GetViewModelFOV( void )
{
	return BaseClass::GetViewModelFOV();
}

int CFF_CL_ClientModeNormal::GetDeathMessageStartHeight( void )
{
	return m_pViewport->GetDeathMessageStartHeight();
}

void CFF_CL_ClientModeNormal::PostRenderVGui()
{
}




