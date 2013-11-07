
/////////////////////////////////////////////////////////////////////////////
// includes
#include "cbase.h"

#include "ff_sh_luabind.h"

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
#include "luabind/out_value_policy.hpp"

// stuff to be bound
#include "ui/ff_cl_luaui_basepanel.h"
#include "hud.h"
#include "hudelement.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace luabind;

void RegisterHudElement( CHudElement *pHudElement )
{
	gHUD.AddHudElement( pHudElement );

	pHudElement->Init();
}

/* Alternate method of creating a Panel, might be needed if Lua starts garbage collecting Panels and we cant adopt them from their constructor
object CreatePanel( lua_State *L, const char *szPanelName )
{
	CFF_CL_LuaUI_BasePanel *pPanel = new CFF_CL_LuaUI_BasePanel( szPanelName );
	object LuaPanel( L, pPanel );
	pPanel->m_luaobjDefinedFunctions = LuaPanel;
	return LuaPanel;
}
*/

void FF_Lua_InitUI( lua_State *L )
{
	Assert(L);

	module(L)
	[
		def( "RegisterHudElement", &RegisterHudElement ),

		class_<CHudElement>("HudElement")
			.def("ListenForGameEvent",				&CHudElement::ListenForGameEvent),

		class_<vgui::Panel>("vguiPanel")
			.def("IsEnabled",						&vgui::Panel::IsEnabled)
			.def("SetEnabled",						&vgui::Panel::SetEnabled)
			.def("IsVisible",						&vgui::Panel::IsVisible)
			.def("SetVisible",						&vgui::Panel::SetVisible)
			.def("SetPaintEnabled",					&vgui::Panel::SetPaintEnabled)
			.def("SetPaintBackgroundEnabled",		&vgui::Panel::SetPaintBackgroundEnabled)
			.def("SetPaintBackgroundType",			&vgui::Panel::SetPaintBackgroundType)
			.def("GetSize",							&vgui::Panel::GetSize, pure_out_value(_2) + pure_out_value(_3))  // _1 is the this pointer
			.def("SetSize",							&vgui::Panel::SetSize)
			.def("GetPos",							&vgui::Panel::GetPos, pure_out_value(_2) + pure_out_value(_3))  // _1 is the this pointer
			.def("SetPos",							&vgui::Panel::SetPos)
			.def("SetEnabled",						&vgui::Panel::SetEnabled),

		class_<CFF_CL_LuaUI_BasePanel, bases<CHudElement, vgui::Panel>>("Panel")
			.def(constructor<lua_State *, const char *>())
	];
}