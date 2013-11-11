#pragma once
#ifndef FF_CL_VGUI_ROOTPANEL_H
#define FF_CL_VGUI_ROOTPANEL_H

#include <vgui_controls/Panel.h>
#include <vgui_controls/EditablePanel.h>
#include "utlvector.h"


class CPanelEffect;


// Serial under of effect, for safe lookup
typedef unsigned int EFFECT_HANDLE;

//-----------------------------------------------------------------------------
// Purpose: Sits between engine and client .dll panels
//  Responsible for drawing screen overlays
//-----------------------------------------------------------------------------
class CFF_CL_VGUI_RootPanel : public vgui::Panel
{
	typedef vgui::Panel BaseClass;
public:
						CFF_CL_VGUI_RootPanel( vgui::VPANEL parent );
	virtual				~CFF_CL_VGUI_RootPanel( void );

	// Draw Panel effects here
	virtual void		PostChildPaint();

	// Clear list of Panel Effects
	virtual void		LevelInit( void );
	virtual void		LevelShutdown( void );

	// Run effects and let them decide whether to remove themselves
	void				OnTick( void );

private:

	// Render all panel effects
	void		RenderPanelEffects( void );

	// List of current panel effects
	CUtlVector< CPanelEffect *> m_Effects;
};


#endif // FF_CL_VGUI_ROOTPANEL_H
