#pragma once
#ifndef FF_CL_LUAUI_BASEPANEL_H
#define FF_CL_LUAUI_BASEPANEL_H

#include "hudelement.h"
#include <vgui_controls/Panel.h>

#include "luabind/object.hpp"

class CFF_CL_LuaUI_BasePanel : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CFF_CL_LuaUI_BasePanel, vgui::Panel );

public:
	CFF_CL_LuaUI_BasePanel( lua_State *L, const char *pElementName );
	// <-- from CHudElement
	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );
	// --> from CHudElement

	// <-- from vgui::Panel
	virtual void OnThink();
	virtual void Paint();
	// --> from vgui::Panel

	void SetProportionalWide( int wide );
	void SetProportionalWide( const char *wstr );
	void SetProportionalTall( int tall );
	void SetProportionalTall( const char *hstr );
	void SetProportionalSize( int w, int h );
	void SetProportionalSize( const char *wstr, const char *hstr );
	
	void SetProportionalPos( int x, int y );
	void SetProportionalPos( const char *xstr, const char *ystr );

	void DrawText( const char *szText, int xpos, int ypos );
	
private:
	luabind::object m_LuaObject;
};

#endif