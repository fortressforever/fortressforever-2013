#pragma once
#ifndef FF_CL_CLIENTMODE_H
#define FF_CL_CLIENTMODE_H

#include "clientmode_shared.h"
#include "ui/ff_cl_hudviewport.h"

class CFF_CL_ClientModeNormal : public ClientModeShared 
{
DECLARE_CLASS( CFF_CL_ClientModeNormal, ClientModeShared );

private:

// IClientMode overrides.
public:

					CFF_CL_ClientModeNormal();
	virtual			~CFF_CL_ClientModeNormal();

	virtual void	InitViewport();

	virtual float	GetViewModelFOV( void );

	int				GetDeathMessageStartHeight( void );

	virtual void	PostRenderVGui();

	
private:
	
	//	void	UpdateSpectatorMode( void );

};


extern IClientMode *GetClientModeNormal();
extern CFF_CL_ClientModeNormal* GetClientModeFFNormal();


#endif // FF_CL_CLIENTMODE_H
