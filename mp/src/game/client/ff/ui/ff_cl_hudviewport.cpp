//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client DLL VGUI2 Viewport
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#pragma warning( disable : 4800  )  // disable forcing int to bool performance warning

// VGUI panel includes
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui/Cursor.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include <vgui/VGUI.h>

// client dll/engine defines
#include "hud.h"
#include <voice_status.h>

// viewport definitions
#include <baseviewport.h>
#include "ff_cl_hudviewport.h"

#include "vguicenterprint.h"
#include "text_message.h"

// HL2MP HUD panels
#include "hl2mpclientscoreboard.h"
#include "hl2mptextwindow.h"


void CFF_CL_HudViewport::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	gHUD.InitColors( pScheme );

	SetPaintBackgroundEnabled( false );
}


IViewPortPanel* CFF_CL_HudViewport::CreatePanelByName(const char *szPanelName)
{
	IViewPortPanel* newpanel = NULL;

	if ( Q_strcmp( PANEL_SCOREBOARD, szPanelName) == 0 )
	{
		newpanel = new CHL2MPClientScoreBoardDialog( this );
		return newpanel;
	}
	else if ( Q_strcmp(PANEL_INFO, szPanelName) == 0 )
	{
		newpanel = new CHL2MPTextWindow( this );
		return newpanel;
	}
	else if ( Q_strcmp(PANEL_SPECGUI, szPanelName) == 0 )
	{
		newpanel = new CHL2MPSpectatorGUI( this );	
		return newpanel;
	}

	// create a generic base panel, don't add twice
	newpanel = BaseClass::CreatePanelByName( szPanelName );

	return newpanel; 
}

void CFF_CL_HudViewport::CreateDefaultPanels( void )
{
	BaseClass::CreateDefaultPanels();
}

int CFF_CL_HudViewport::GetDeathMessageStartHeight( void )
{
	int x = YRES(2);

	IViewPortPanel *spectator = gViewPortInterface->FindPanelByName( PANEL_SPECGUI );

	//TODO: Link to actual height of spectator bar
	if ( spectator && spectator->IsVisible() )
	{
		x += YRES(52);
	}

	return x;
}

