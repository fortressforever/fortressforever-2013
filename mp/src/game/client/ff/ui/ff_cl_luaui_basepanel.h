#pragma once
#ifndef FF_CL_LUAUI_BASEPANEL_H
#define FF_CL_LUAUI_BASEPANEL_H

#include "hudelement.h"
#include <vgui_controls/Panel.h>
#include "vgui_controls/EditablePanel.h"

#include "luabind/object.hpp"

class CFF_CL_LuaUI_BasePanel : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CFF_CL_LuaUI_BasePanel, vgui::EditablePanel );

public:
	CFF_CL_LuaUI_BasePanel( lua_State *L, const char *pElementName );
	~CFF_CL_LuaUI_BasePanel();

	// <-- from CHudElement
	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );
	// --> from CHudElement

	// <-- from vgui::Panel
	virtual void OnThink();
	virtual void Paint();
	virtual void PerformLayout();
	// --> from vgui::Panel

	// --> based on vgui::Frame
	virtual void SetupResizeCursors();
	// <-- based on vgui::Frame
	
	void SetEditingControlsVisible( bool bState );

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
	
	Panel		*_topGrip;
	Panel		*_bottomGrip;
	Panel		*_leftGrip;
	Panel		*_rightGrip;
	Panel		*_topLeftGrip;
	Panel		*_topRightGrip;
	Panel		*_bottomLeftGrip;
	Panel		*_bottomRightGrip;
	Panel		*_extentsGrip;
};

#endif