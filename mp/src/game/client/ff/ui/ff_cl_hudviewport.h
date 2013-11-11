#pragma once
#ifndef FF_CL_HUDVIEWPORT_H
#define FF_CL_HUDVIEWPORT_H

#include "baseviewport.h"

using namespace vgui;

namespace vgui 
{
	class Panel;
}

class CFF_CL_HudViewport : public CBaseViewport
{

private:
	DECLARE_CLASS_SIMPLE( CFF_CL_HudViewport, CBaseViewport );

public:

	IViewPortPanel* CreatePanelByName(const char *szPanelName);
	void CreateDefaultPanels( void );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
		
	int GetDeathMessageStartHeight( void );
};


#endif // FF_CL_HUDVIEWPORT_H
