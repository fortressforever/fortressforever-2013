#include "cbase.h"
#include "ui/ff_cl_luaui_basepanel.h"

#include "ff_cl_scriptman_ui.h"

#include "iclientmode.h"
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui/IInput.h>
#include <vgui_controls/Panel.h>
/*
#include "hud.h"
#include "hud_macros.h"
#include "view.h"

#include <KeyValues.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>

*/
#include "baseviewport.h"
#include "ff_cl_hudviewport.h"

#include "luabind/luabind.hpp"
#include "luabind/object.hpp"

#include "vgui_int.h"

#include "ff_sh_luautil.h"

using namespace vgui;
using namespace luabind;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static bool g_LuaUI_bEditingEnabled = false;

// number of pixels wide/tall the resizing draggers are when in editing mode
#define DRAGGER_SIZE 8
// number of pixels distance before the frame will snap to an edge
#define DEFAULT_SNAP_RANGE 10

CON_COMMAND(hudedit, "Toggles HUD edit mode")
{
	vgui::Panel *pViewport = g_pClientMode->GetViewport();
	CFF_CL_HudViewport *pHudViewport = static_cast<CFF_CL_HudViewport *>(pViewport);

	if (!g_LuaUI_bEditingEnabled)
	{
		pHudViewport->SetMouseInputEnabled( true );
		//pHudViewport->SetKeyBoardInputEnabled( true );

		Msg("HUD Editing Mode enabled\n");
		g_LuaUI_bEditingEnabled = true;
	}
	else
	{
		pHudViewport->SetMouseInputEnabled( false );
		//pHudViewport->SetKeyBoardInputEnabled( false );

		Msg("HUD Editing Mode disabled\n");
		g_LuaUI_bEditingEnabled = false;
	}
};

/*
	Note: GripPanel code taken from vgui::Frame

	FF TODO: Move this code elsewhere
*/
namespace
{
	//-----------------------------------------------------------------------------
	// Purpose: Panel to handle dragging/resizing frames
	//-----------------------------------------------------------------------------
	class GripPanel : public Panel
	{
	public:
		GripPanel(Panel *dragFrame, const char *name, int xdir, int ydir) : Panel(dragFrame, name)
		{
			_frame = dragFrame;
			_dragging = false;
			_dragMultX = xdir;
			_dragMultY = ydir;
			SetPaintEnabled(true);
			SetPaintBackgroundEnabled(true);
			SetPaintBorderEnabled(false);
			m_iSnapRange = DEFAULT_SNAP_RANGE;

			if (xdir == 1 && ydir == 1)
			{
				// bottom-right grip gets an image
				SetPaintEnabled(true);
				SetPaintBackgroundEnabled(true);
			}

			SetBlockDragChaining( true );
		}
		
		// Purpose- handle window resizing
		// Input- dx, dy, the offet of the mouse pointer from where we started dragging
		virtual void moved(int dx, int dy)
		{
			// FF
			//if (!_frame->IsSizeable())
			//	return;
			
			// Start off with x, y at the coords of where we started to drag
			int newX = _dragOrgPos[0], newY =_dragOrgPos[1];
			// Start off with width and tall equal from window when we started to drag
			int newWide = _dragOrgSize[0], newTall = _dragOrgSize[1];
			
			// get window's minimum size
			int minWide, minTall;
			_frame->GetMinimumSize( minWide, minTall);
			
			// Handle  width resizing
			newWide += (dx * _dragMultX);
			// Handle the position of the corner x position
			if (_dragMultX == -1)
			{
				// only move if we are not at the minimum
				// if we are at min we have to force the proper offset (dx)
				if (newWide < minWide)
				{
					dx=_dragOrgSize[0]-minWide;
				}
				newX += dx;	  // move window to its new position
			}
			
			// Handle height resizing
			newTall += (dy * _dragMultY);
			// Handle position of corner y position
			if (_dragMultY == -1)
			{
				if (newTall < minTall)
				{
					dy=_dragOrgSize[1]-minTall;
				}
				newY += dy;
			}
			
			/* FF
			if ( _frame->GetClipToParent() )
			{
				// If any coordinate is out of range, snap it back
				if ( newX < 0 )
					newX = 0;
				if ( newY < 0 )
					newY = 0;
				
				int sx, sy;
				surface()->GetScreenSize( sx, sy );

				int w, h;
				_frame->GetSize( w, h );
				if ( newX + w > sx )
				{
					newX = sx - w;
				}
				if ( newY + h > sy )
				{
					newY = sy - h;
				}
			}
			*/

			// set new position
			_frame->SetPos(newX, newY);
			// set the new size			
			// if window is below min size it will automatically pop to min size
			_frame->SetSize(newWide, newTall);
			_frame->InvalidateLayout();
			_frame->Repaint();
		}
		
		void OnCursorMoved(int x, int y)
		{
			if (!_dragging)
				return;

			if (!input()->IsMouseDown(MOUSE_LEFT))
			{
				// for some reason we're marked as dragging when the mouse is released
				// trigger a release
				OnMouseReleased(MOUSE_LEFT);
				return;
			}

			input()->GetCursorPos(x, y);
			moved((x - _dragStart[0]), ( y - _dragStart[1]));
			_frame->Repaint();
		}
		
		void OnMousePressed(MouseCode code)
		{
			if (code == MOUSE_LEFT)
			{ 
				_dragging=true;
				int x,y;
				input()->GetCursorPos(x,y);
				_dragStart[0]=x;
				_dragStart[1]=y;
				_frame->GetPos(_dragOrgPos[0],_dragOrgPos[1]);
				_frame->GetSize(_dragOrgSize[0],_dragOrgSize[1]);
				input()->SetMouseCapture(GetVPanel());
				
				// if a child doesn't have focus, get it for ourselves
				VPANEL focus = input()->GetFocus();
				if (!focus || !ipanel()->HasParent(focus, _frame->GetVPanel()))
				{
					_frame->RequestFocus();
				}
				_frame->Repaint();
			}
			else
			{
				GetParent()->OnMousePressed(code);
			}
		}

		void OnMouseDoublePressed(MouseCode code)
		{
			GetParent()->OnMouseDoublePressed(code);
		}

		void PaintBackground()
		{
			if (!::g_LuaUI_bEditingEnabled) return;

			if (_dragMultX != 0 && _dragMultY != 0)
			{
				int w,h;
				GetSize( w, h );
				DrawHollowBox( 0,0,w,h,Color(0,0,255,255),1.0f,1,1 );
			}
		}
		
		void OnMouseReleased(MouseCode code)
		{
			_dragging = false;
			input()->SetMouseCapture(NULL);
		}

		void OnMouseCaptureLost()
		{
			Panel::OnMouseCaptureLost();
			_dragging = false;
		}

	protected:
		Panel *_frame;
		int  _dragMultX;
		int  _dragMultY;
		bool _dragging;
		int  _dragOrgPos[2];
		int  _dragOrgSize[2];
		int  _dragStart[2];
		int  m_iSnapRange;
	};
	
	//-----------------------------------------------------------------------------
	// Purpose: Handles caption grip input for moving dialogs around
	//-----------------------------------------------------------------------------
	class MoveableGripPanel : public GripPanel
	{
	public:
		MoveableGripPanel(Panel* frame, const char *name) : GripPanel(frame, name, 0, 0)
		{
		}
		
		void PaintBackground()
		{
			if (!::g_LuaUI_bEditingEnabled) return;

			int w,h;
			GetSize( w, h );
			DrawHollowBox( 0,0,w,h,Color(0,255,0,255),1.0f,1,1 );
		}

		void moved(int dx, int dy)
		{
			// FF
			//if (!_frame->IsMoveable())
			//	return;

			int newX = _dragOrgPos[0] + dx;
			int newY = _dragOrgPos[1] + dy;

			if (m_iSnapRange)
			{
				// first check docking to desktop
				int wx, wy, ww, wt;
				surface()->GetWorkspaceBounds(wx, wy, ww, wt);
				getInsideSnapPosition(wx, wy, ww, wt, newX, newY);

				// now lets check all windows and see if we snap to those
				// root panel
				VPANEL root = surface()->GetEmbeddedPanel();
				// cycle through panels
				// look for panels that are visible and are popups that we can dock to
				for (int i = 0; i < ipanel()->GetChildCount(root); ++i)
				{
					VPANEL child = ipanel()->GetChild(root, i);
					tryToDock (child, newX, newY);
				}
			}

			/* FF
			if ( _frame->GetClipToParent() )
			{
				// If any coordinate is out of range, snap it back
				if ( newX < 0 )
					newX = 0;
				if ( newY < 0 )
					newY = 0;
				
				int sx, sy;
				surface()->GetScreenSize( sx, sy );

				int w, h;
				_frame->GetSize( w, h );
				if ( newX + w > sx )
				{
					newX = sx - w;
				}
				if ( newY + h > sy )
				{
					newY = sy - h;
				}
			}
			*/

			_frame->SetPos(newX, newY);

		}
		
		void tryToDock(VPANEL window, int &newX, int & newY)
		{
			// bail if child is this window	
			if ( window == _frame->GetVPanel())
				return;
			
			int cx, cy, cw, ct;
			if ( (ipanel()->IsVisible(window)) && (ipanel()->IsPopup(window)) )
			{
				// position
				ipanel()->GetAbsPos(window, cx, cy);
				// dimensions
				ipanel()->GetSize(window, cw, ct);
				bool snapped = getOutsideSnapPosition (cx, cy, cw, ct, newX, newY);
				if (snapped)
				{ 
					// if we snapped, we're done with this path
					// dont try to snap to kids
					return;
				}
			}

			// check all children
			for (int i = 0; i < ipanel()->GetChildCount(window); ++i)
			{
				VPANEL child = ipanel()->GetChild(window, i);
				tryToDock(child, newX, newY);
			}

		}

		// Purpose: To calculate the windows new x,y position if it snaps
		//          Will snap to the INSIDE of a window (eg desktop sides
		// Input: boundX boundY, position of candidate window we are seeing if we snap to
		//        boundWide, boundTall, width and height of window we are seeing if we snap to
		// Output: snapToX, snapToY new coords for window, unchanged if we dont snap
		// Returns true if we snapped, false if we did not snap.
		bool getInsideSnapPosition(int boundX, int boundY, int boundWide, int boundTall,
			int &snapToX, int &snapToY)
		{
			
			int wide, tall;
			_frame->GetSize(wide, tall);
			Assert (wide > 0);
			Assert (tall > 0);
			
			bool snapped=false;
			if (abs(snapToX - boundX) < m_iSnapRange)
			{
				snapToX = boundX;
				snapped=true;
			}
			else if (abs((snapToX + wide) - (boundX + boundWide)) < m_iSnapRange)
			{
				snapToX = boundX + boundWide - wide;
				snapped=true;
			}

			if (abs(snapToY - boundY) < m_iSnapRange)
			{
				snapToY = boundY;
				snapped=true;
			}
			else if (abs((snapToY + tall) - (boundY + boundTall)) < m_iSnapRange)
			{
				snapToY = boundY + boundTall - tall;
				snapped=true;
			}
			return snapped;
			
		}

		// Purpose: To calculate the windows new x,y position if it snaps
		//          Will snap to the OUTSIDE edges of a window (i.e. will stick peers together
		// Input: left, top, position of candidate window we are seeing if we snap to
		//        boundWide, boundTall, width and height of window we are seeing if we snap to
		// Output: snapToX, snapToY new coords for window, unchanged if we dont snap
		// Returns true if we snapped, false if we did not snap.
		bool getOutsideSnapPosition(int left, int top, int boundWide, int boundTall,
			int &snapToX, int &snapToY)
		{
			Assert (boundWide >= 0);
			Assert (boundTall >= 0);
						
			bool snapped=false;
			
			int right=left+boundWide;
			int bottom=top+boundTall;

			int wide, tall;
			_frame->GetSize(wide, tall);
			Assert (wide > 0);
			Assert (tall > 0);

			// we now see if we are going to be able to snap to a window side, and not
			// just snap to the "open air"
			// want to make it so that if any part of the window can dock to the candidate, it will

			// is this window horizontally snappable to the candidate
			bool horizSnappable=( 
				//  top of window is in range
				((snapToY > top) && (snapToY < bottom)) 
				// bottom of window is in range
				|| ((snapToY+tall > top) && (snapToY+tall < bottom)) 
				// window is just plain bigger than the window we wanna dock to
				|| ((snapToY < top) && (snapToY+tall > bottom)) ); 
			
			
			// is this window vertically snappable to the candidate
			bool vertSnappable=	( 
				 //  left of window is in range
				((snapToX > left) && (snapToX < right))
				//  right of window is in range
				|| ((snapToX+wide > left) && (snapToX+wide < right)) 
				// window is just plain bigger than the window we wanna dock to
				|| ((snapToX < left) && (snapToX+wide > right)) ); 
			
			// if neither, might as well bail
			if ( !(horizSnappable || vertSnappable) )
				return false;

			//if we're within the snap threshold then snap
			if ( (snapToX <= (right+m_iSnapRange)) && 
				(snapToX >= (right-m_iSnapRange)) ) 
			{  
				if (horizSnappable)
				{
					//disallow "open air" snaps
					snapped=true;
					snapToX = right;  
				}
			}
			else if ((snapToX + wide) >= (left-m_iSnapRange) &&
				(snapToX + wide) <= (left+m_iSnapRange)) 
			{
				if (horizSnappable)
				{
					snapped=true;
					snapToX = left-wide;
				}
			}
			
			if ( (snapToY <= (bottom+m_iSnapRange)) &&
				(snapToY >= (bottom-m_iSnapRange)) ) 
			{
				if (vertSnappable)
				{
					snapped=true;
					snapToY = bottom;
				}
			}
			else if ((snapToY + tall) <= (top+m_iSnapRange) &&
				(snapToY + tall) >= (top-m_iSnapRange)) 
			{
				if (vertSnappable)
				{
					snapped=true;
					snapToY = top-tall;
				}
			}
			return snapped;
		}
	};
	
}

/// this should only be called from Lua
CFF_CL_LuaUI_BasePanel::CFF_CL_LuaUI_BasePanel( lua_State *L, const char *pElementName ) : CHudElement( pElementName ), BaseClass(NULL, "HudLuaUIPanel")
{
	Assert( L );

	// the userdata object is on the top of the stack
	m_LuaObject = object(from_stack(L, 1));
	
	SetParent(g_pClientMode->GetViewport());
	SetProportional( true );
	SetMouseInputEnabled( true );
	SetMinimumSize( DRAGGER_SIZE, DRAGGER_SIZE );
	
	// add dragging grips
	_extentsGrip = new MoveableGripPanel(this, "luaui_panel_extentsGrip" );

	_topGrip = new GripPanel(this, "luaui_panel_topGrip", 0, -1);
	_bottomGrip = new GripPanel(this, "luaui_panel_bottomGrip", 0, 1);
	_leftGrip = new GripPanel(this, "luaui_panel_leftGrip", -1, 0);
	_rightGrip = new GripPanel(this, "luaui_panel_rightGrip", 1, 0);
	_topLeftGrip = new GripPanel(this, "luaui_panel_tlGrip", -1, -1);
	_topRightGrip = new GripPanel(this, "luaui_panel_trGrip", 1, -1);
	_bottomLeftGrip = new GripPanel(this, "luaui_panel_blGrip", -1, 1);
	_bottomRightGrip = new GripPanel(this, "luaui_panel_brGrip", 1, 1);
	
	SetupResizeCursors();
	//SetEditingControlsVisible( false );
}

CFF_CL_LuaUI_BasePanel::~CFF_CL_LuaUI_BasePanel()
{
	delete _extentsGrip;
	
	delete _topGrip;
	delete _bottomGrip;
	delete _leftGrip;
	delete _rightGrip;
	delete _topLeftGrip;
	delete _topRightGrip;
	delete _bottomLeftGrip;
	delete _bottomRightGrip;
}

/// from CHudElement
void CFF_CL_LuaUI_BasePanel::Init()
{
	Reset();

	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["Init"] ) == LUA_TFUNCTION )
	{
		try
		{
			luabind::call_function<void>( m_LuaObject["Init"], this );
		}
		catch( error& e )
		{
			object error_msg(from_stack(e.state(), -1));
			g_UIScriptManager.LuaWarning( luabind::object_cast<const char*>( error_msg ) );
		}
	}
}

/// from CHudElement
void CFF_CL_LuaUI_BasePanel::Reset()
{
	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["Reset"] ) == LUA_TFUNCTION )
	{
		try
		{
			luabind::call_function<void>( m_LuaObject["Reset"], this );
		}
		catch( error& e )
		{
			object error_msg(from_stack(e.state(), -1));
			g_UIScriptManager.LuaWarning( "%s\n", luabind::object_cast<const char*>( error_msg ) );
		}
	}
}

/// from CHudElement
void CFF_CL_LuaUI_BasePanel::VidInit()
{
	Reset();
	
	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["VidInit"] ) == LUA_TFUNCTION )
	{
		try
		{
			luabind::call_member<HFont>(m_LuaObject, "VidInit");
		}
		catch( error& e )
		{
			object error_msg(from_stack(e.state(), -1));
			g_UIScriptManager.LuaWarning( "%s\n", luabind::object_cast<const char*>( error_msg ) );
		}
	}
}

/// from vgui::Panel
void CFF_CL_LuaUI_BasePanel::OnThink()
{
	BaseClass::OnThink();

	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["OnThink"] ) == LUA_TFUNCTION )
	{
		try
		{
			luabind::call_function<void>( m_LuaObject["OnThink"], this );
		}
		catch( error& e )
		{
			object error_msg(from_stack(e.state(), -1));
			g_UIScriptManager.LuaWarning( "%s\n", luabind::object_cast<const char*>( error_msg ) );
		}
	}
}

/// from vgui::Panel
void CFF_CL_LuaUI_BasePanel::Paint()
{
	BaseClass::Paint();

	if ( m_LuaObject.is_valid() && luabind::type( m_LuaObject["Paint"] ) == LUA_TFUNCTION )
	{
		try
		{
			luabind::call_function<void>( m_LuaObject["Paint"], this );
		}
		catch( error& e )
		{
			object error_msg(from_stack(e.state(), -1));
			g_UIScriptManager.LuaWarning( "%s\n", luabind::object_cast<const char*>( error_msg ) );
		}
	}
}

void CFF_CL_LuaUI_BasePanel::SetEditingControlsVisible( bool bState )
{
	_extentsGrip->SetVisible( bState );
	_topGrip->SetVisible( bState );
	_leftGrip->SetVisible( bState );
	_topLeftGrip->SetVisible( bState );
	_topRightGrip->SetVisible( bState );
	_bottomLeftGrip->SetVisible( bState );
	_bottomGrip->SetVisible( bState );
	_rightGrip->SetVisible( bState );
	_bottomRightGrip->SetVisible( bState );
}

/// Reposition child elements when the size of the panel changes
void CFF_CL_LuaUI_BasePanel::PerformLayout()
{
	// chain back
	BaseClass::PerformLayout();
	
	// move everything into place
	int wide, tall;
	GetSize(wide, tall);
	
	_extentsGrip->SetBounds(0,0,wide,tall);
	_extentsGrip->MoveToFront();

	_topGrip->SetBounds(DRAGGER_SIZE, 0, wide - DRAGGER_SIZE, DRAGGER_SIZE);
	_leftGrip->SetBounds(0, DRAGGER_SIZE, DRAGGER_SIZE, tall - DRAGGER_SIZE);
	_topLeftGrip->SetBounds(0, 0, DRAGGER_SIZE, DRAGGER_SIZE);
	_topRightGrip->SetBounds(wide - DRAGGER_SIZE, 0, DRAGGER_SIZE, DRAGGER_SIZE);
	_bottomLeftGrip->SetBounds(0, tall - DRAGGER_SIZE, DRAGGER_SIZE, DRAGGER_SIZE);
	_bottomGrip->SetBounds(DRAGGER_SIZE, tall - DRAGGER_SIZE, wide - DRAGGER_SIZE, DRAGGER_SIZE);
	_rightGrip->SetBounds(wide - DRAGGER_SIZE, DRAGGER_SIZE, DRAGGER_SIZE, tall - DRAGGER_SIZE);
	_bottomRightGrip->SetBounds(wide - DRAGGER_SIZE, tall - DRAGGER_SIZE, DRAGGER_SIZE, DRAGGER_SIZE);

	_topGrip->MoveToFront();
	_bottomGrip->MoveToFront();
	_leftGrip->MoveToFront();
	_rightGrip->MoveToFront();
	_topLeftGrip->MoveToFront();
	_topRightGrip->MoveToFront();
	_bottomLeftGrip->MoveToFront();
	_bottomRightGrip->MoveToFront();
}

/// Set the cursors for resize grips
void CFF_CL_LuaUI_BasePanel::SetupResizeCursors()
{
	_extentsGrip->SetCursor(dc_sizeall);

	_topGrip->SetCursor(dc_sizens);
	_bottomGrip->SetCursor(dc_sizens);
	_leftGrip->SetCursor(dc_sizewe);
	_rightGrip->SetCursor(dc_sizewe);
	_topLeftGrip->SetCursor(dc_sizenwse);
	_topRightGrip->SetCursor(dc_sizenesw);
	_bottomLeftGrip->SetCursor(dc_sizenesw);
	_bottomRightGrip->SetCursor(dc_sizenwse);
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

	/*
	int parentX = 0;
	int parentY = 0;

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