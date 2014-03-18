
/////////////////////////////////////////////////////////////////////////////
// includes
#include "cbase.h"

#include "ff_sh_luabind.h"

// dexter note 10/29/2013 these are definitely still needed for lua/luabind
// later clarification - the undefs are needed, similiar to other min/max clashes on GCC
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
#include "ff_cl_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace luabind;

namespace FFLuaLib_UI
{

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

	float GetSpeed( const C_BaseEntity *pEntity )
	{
		return pEntity->GetAbsVelocity().Length2D();
	}

	CFF_CL_Player *GetLocalPlayer()
	{
		return CFF_CL_Player::GetLocalFFPlayer();
	}

};

void FF_Lua_InitUI( lua_State *L )
{
	Assert(L);

	module(L)
	[
		// globals
		def( "RegisterHudElement",					&FFLuaLib_UI::RegisterHudElement ),

		// UI elements
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
			.def("SetRawSize",						&vgui::Panel::SetSize)
			.def("GetPos",							&vgui::Panel::GetPos, pure_out_value(_2) + pure_out_value(_3))  // _1 is the this pointer
			.def("SetRawPos",						&vgui::Panel::SetPos)
			.def("SetEnabled",						&vgui::Panel::SetEnabled),

		class_<CFF_CL_LuaUI_BasePanel, bases<CHudElement, vgui::Panel> >("Panel")
			.def(constructor<lua_State *, const char *>())
			.def("SetPos",							(void(CFF_CL_LuaUI_BasePanel::*)(const char*, const char*))&CFF_CL_LuaUI_BasePanel::SetProportionalPos)
			.def("SetPos",							(void(CFF_CL_LuaUI_BasePanel::*)(int, int))&CFF_CL_LuaUI_BasePanel::SetProportionalPos)
			.def("SetWide",							(void(CFF_CL_LuaUI_BasePanel::*)(const char*))&CFF_CL_LuaUI_BasePanel::SetProportionalWide)
			.def("SetWide",							(void(CFF_CL_LuaUI_BasePanel::*)(int))&CFF_CL_LuaUI_BasePanel::SetProportionalWide)
			.def("SetTall",							(void(CFF_CL_LuaUI_BasePanel::*)(const char*))&CFF_CL_LuaUI_BasePanel::SetProportionalTall)
			.def("SetTall",							(void(CFF_CL_LuaUI_BasePanel::*)(int))&CFF_CL_LuaUI_BasePanel::SetProportionalTall)
			.def("SetSize",							(void(CFF_CL_LuaUI_BasePanel::*)(const char*, const char*))&CFF_CL_LuaUI_BasePanel::SetProportionalSize)
			.def("SetSize",							(void(CFF_CL_LuaUI_BasePanel::*)(int, int))&CFF_CL_LuaUI_BasePanel::SetProportionalSize)
			.def("DrawText",						&CFF_CL_LuaUI_BasePanel::DrawText)
			.def("DrawBox",							&CFF_CL_LuaUI_BasePanel::DrawBox),

		// base entity
		class_<C_BaseEntity>("BaseEntity")
			.def("GetVelocity",						&C_BaseEntity::GetAbsVelocity)
			.def("GetSpeed",						&FFLuaLib_UI::GetSpeed),

		class_<C_BasePlayer, C_BaseEntity>("BasePlayer"),

		class_<CFF_CL_Player, C_BasePlayer>("Player")
			.def("GetMaxSpeed",						&CFF_CL_Player::MaxSpeed)
			.def("GetHealth",						&CFF_CL_Player::GetHealth)
			.def("GetMaxHealth",					&CFF_CL_Player::GetMaxHealth),

		def("LocalPlayer",							&FFLuaLib_UI::GetLocalPlayer)
	];
}
