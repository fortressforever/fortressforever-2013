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

void CFF_CL_LuaUI_BasePanel::DrawText( const char *szText, int xpos, int ypos )
{
	wchar_t wszText[255];
	g_pVGuiLocalize->ConvertANSIToUnicode( szText, wszText, sizeof(wszText) );

	IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
	Assert( pScheme );

	HFont font = pScheme->GetFont( "Default", IsProportional() );

	surface()->DrawSetTextFont(font);
	surface()->DrawSetTextColor(GetFgColor());
	surface()->DrawSetTextPos(xpos, ypos);
	surface()->DrawUnicodeString( wszText );
}