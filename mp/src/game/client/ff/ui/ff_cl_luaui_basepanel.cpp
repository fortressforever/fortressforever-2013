#include "cbase.h"
#include "ui/ff_cl_luaui_basepanel.h"

#include "ff_cl_scriptman_ui.h"

#include "iclientmode.h"
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
/*
#include "hud.h"
#include "hud_macros.h"
#include "view.h"

#include <KeyValues.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>

*/

#include "luabind/luabind.hpp"
#include "luabind/object.hpp"

#include "vgui_int.h"

#include "ff_sh_luautil.h"

using namespace vgui;
using namespace luabind;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/// this should only be called from Lua
CFF_CL_LuaUI_BasePanel::CFF_CL_LuaUI_BasePanel( lua_State *L, const char *pElementName ) : CHudElement( pElementName ), vgui::Panel(NULL, "HudLuaUIPanel")
{
	Assert( L );

	// the userdata object is on the top of the stack
	m_LuaObject = object(from_stack(L, 1));
	
	SetParent(g_pClientMode->GetViewport());
	SetProportional( true );
}

/// from CHudElement
void CFF_CL_LuaUI_BasePanel::Init()
{
	Reset();

	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["Init"] ) == LUA_TFUNCTION )
	{
		luabind::call_function<void>( m_LuaObject["Init"], this );
	}
}

/// from CHudElement
void CFF_CL_LuaUI_BasePanel::Reset()
{
	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["Reset"] ) == LUA_TFUNCTION )
	{
		luabind::call_function<void>( m_LuaObject["Reset"], this );
	}
}

/// from CHudElement
void CFF_CL_LuaUI_BasePanel::VidInit()
{
	Reset();
	
	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["VidInit"] ) == LUA_TFUNCTION )
	{
		luabind::call_function<void>( m_LuaObject["VidInit"], this );
	}
}

/// from vgui::Panel
void CFF_CL_LuaUI_BasePanel::OnThink()
{
	BaseClass::OnThink();

	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["OnThink"] ) == LUA_TFUNCTION )
	{
		luabind::call_function<void>( m_LuaObject["OnThink"], this );
	}
}

/// from vgui::Panel
void CFF_CL_LuaUI_BasePanel::Paint()
{
	BaseClass::Paint();

	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["Paint"] ) == LUA_TFUNCTION )
	{
		luabind::call_function<void>( m_LuaObject["Paint"], this );
	}
}

/// Drawtext helper function
void CFF_CL_LuaUI_BasePanel::DrawText( const char *szText, int xpos, int ypos )
{
	wchar_t wszText[255];
	g_pVGuiLocalize->ConvertANSIToUnicode( szText, wszText, sizeof(wszText) );

	IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
	Assert( pScheme );

	HFont font = pScheme->GetFont( "Default", IsProportional() );

	surface()->DrawSetTextFont(font);
	surface()->DrawSetTextColor(GetFgColor());

	if (IsProportional())
	{
		xpos = scheme()->GetProportionalScaledValueEx(GetScheme(), xpos);
		ypos = scheme()->GetProportionalScaledValueEx(GetScheme(), ypos);
	}

	surface()->DrawSetTextPos(xpos, ypos);
	surface()->DrawUnicodeString( wszText );
}

/*
==========================================================
 Note:	Proportional width/height/pos code is based on 
		vgui::Panel::ApplySettings in Panel.cpp

 FF TODO: Rewrite this to be more Lua-friendly (get rid of
			string-based alignment settings)
==========================================================
*/

/// Set width proportionally
void CFF_CL_LuaUI_BasePanel::SetProportionalWide( int wide ) 
{
	if ( IsProportional() )
	{
		// scale the width up to our screen co-ords
		wide = scheme()->GetProportionalScaledValueEx(GetScheme(), wide);
	}

	BaseClass::SetWide( wide ); 
}

/// Set width proportionally with an optional 'f' prefix to fill the parent
void CFF_CL_LuaUI_BasePanel::SetProportionalWide( const char *wstr )
{
	int iScreenWide, iScreenTall;
	surface()->GetScreenSize( iScreenWide, iScreenTall );
	
	_buildModeFlags &= ~(BUILDMODE_SAVE_WIDE_FULL);

	if (wstr[0] == 'f' || wstr[0] == 'F')
	{
		_buildModeFlags |= BUILDMODE_SAVE_WIDE_FULL;
		wstr++;
	}
	int wide = atoi(wstr);
	// now correct the alignment
	if (_buildModeFlags & BUILDMODE_SAVE_WIDE_FULL)
	{
		wide = iScreenWide - wide;
	}

	SetProportionalWide( wide );
}

/// Set height proportionally
void CFF_CL_LuaUI_BasePanel::SetProportionalTall( int tall )
{
	if ( IsProportional() )
	{
		// scale the height up to our screen co-ords
		tall = scheme()->GetProportionalScaledValueEx(GetScheme(), tall);
	}

	BaseClass::SetTall( tall );
}

/// Set height proportionally with an optional 'f' prefix to fill the parent
void CFF_CL_LuaUI_BasePanel::SetProportionalTall( const char *hstr )
{
	int iScreenWide, iScreenTall;
	surface()->GetScreenSize( iScreenWide, iScreenTall );
	
	_buildModeFlags &= ~(BUILDMODE_SAVE_TALL_FULL);

	// allow tall to be use the "fill" option, set to the height of the parent/screen
	if (hstr[0] == 'f' || hstr[0] == 'F')
	{
		_buildModeFlags |= BUILDMODE_SAVE_TALL_FULL;
		hstr++;
	}
	int tall = atoi(hstr);
	// now correct the alignment
	if (_buildModeFlags & BUILDMODE_SAVE_TALL_FULL)
	{
		tall = iScreenTall - tall;
	}
	
	SetProportionalTall( tall );
}

/// Set size proportionally
void CFF_CL_LuaUI_BasePanel::SetProportionalSize( int w, int h )
{
	SetProportionalWide( w );
	SetProportionalTall( h );
}

/// Set size proportionally with optional 'f' prefixes to fill the parent
void CFF_CL_LuaUI_BasePanel::SetProportionalSize( const char *wstr, const char *hstr )
{
	SetProportionalWide( wstr );
	SetProportionalTall( hstr );
}

/// Set position proportionally
void CFF_CL_LuaUI_BasePanel::SetProportionalPos( int x, int y )
{
	// scale the x up to our screen co-ords
	if ( IsProportional() )
	{
		x = scheme()->GetProportionalScaledValueEx(GetScheme(), x);
		y = scheme()->GetProportionalScaledValueEx(GetScheme(), y);
	}

	BaseClass::SetPos( x, y );
}

void CFF_CL_LuaUI_BasePanel::SetProportionalPos( const char *xstr, const char *ystr )
{
	// clear any alignment flags
	_buildModeFlags &= ~(BUILDMODE_SAVE_XPOS_RIGHTALIGNED | BUILDMODE_SAVE_XPOS_CENTERALIGNED | BUILDMODE_SAVE_YPOS_BOTTOMALIGNED | BUILDMODE_SAVE_YPOS_CENTERALIGNED | BUILDMODE_SAVE_PROPORTIONAL_TO_PARENT);

	// get the position
	int alignScreenWide, alignScreenTall;	// screen dimensions used for pinning in splitscreen
	surface()->GetScreenSize( alignScreenWide, alignScreenTall );
	
	int screenWide = alignScreenWide;
	int screenTall = alignScreenTall;

	// temporarily remove the override to get the fullscreen dimensions
	if ( surface()->IsScreenSizeOverrideActive() )
	{
		surface()->ForceScreenSizeOverride( false, 0, 0 );
		surface()->GetScreenSize( screenWide, screenTall );

		// restore the override
		surface()->ForceScreenSizeOverride( true, alignScreenWide, alignScreenTall );
	}

	int parentX = 0;
	int parentY = 0;

	/*
	// flag to cause windows to get screenWide and screenTall from their parents,
	// this allows children windows to use fill and right/bottom alignment even
	// if their parent does not use the full screen.
	if ( inResourceData->GetInt( "proportionalToParent", 0 ) == 1 )
	{
		_buildModeFlags |= BUILDMODE_SAVE_PROPORTIONAL_TO_PARENT;
		if ( GetParent() != NULL )
		{
			GetParent()->GetBounds( parentX, parentY, alignScreenWide, alignScreenTall );
		}
	}
	*/

	int x, y;
	GetPos(x, y);
	
	if (xstr)
	{
		// look for alignment flags
		if (xstr[0] == 'r' || xstr[0] == 'R')
		{
			_buildModeFlags |= BUILDMODE_SAVE_XPOS_RIGHTALIGNED;
			xstr++;
		}
		else if (xstr[0] == 'c' || xstr[0] == 'C')
		{
			_buildModeFlags |= BUILDMODE_SAVE_XPOS_CENTERALIGNED;
			xstr++;
		}

		// get the value
		x = atoi(xstr);
		
		// scale the x up to our screen co-ords
		if ( IsProportional() )
		{
			x = scheme()->GetProportionalScaledValueEx(GetScheme(), x);
			y = scheme()->GetProportionalScaledValueEx(GetScheme(), y);
		}
		// now correct the alignment
		if (_buildModeFlags & BUILDMODE_SAVE_XPOS_RIGHTALIGNED)
		{
			x = alignScreenWide - x;
		}
		else if (_buildModeFlags & BUILDMODE_SAVE_XPOS_CENTERALIGNED)
		{
			x = (alignScreenWide / 2) + x;
		}
	}

	if (ystr)
	{
		// look for alignment flags
		if (ystr[0] == 'r' || ystr[0] == 'R')
		{
			_buildModeFlags |= BUILDMODE_SAVE_YPOS_BOTTOMALIGNED;
			ystr++;
		}
		else if (ystr[0] == 'c' || ystr[0] == 'C')
		{
			_buildModeFlags |= BUILDMODE_SAVE_YPOS_CENTERALIGNED;
			ystr++;
		}
		y = atoi(ystr);
		if (IsProportional())
		{
			// scale the y up to our screen co-ords
			y = scheme()->GetProportionalScaledValueEx(GetScheme(), y);
		}
		// now correct the alignment
		if (_buildModeFlags & BUILDMODE_SAVE_YPOS_BOTTOMALIGNED)
		{
			y = alignScreenTall - y;
		}
		else if (_buildModeFlags & BUILDMODE_SAVE_YPOS_CENTERALIGNED)
		{
			y = (alignScreenTall / 2) + y;
		}
	}

	BaseClass::SetPos( x, y );
}