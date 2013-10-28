#ifndef FF_SH_GAMEMOVEMENT_H
#define FF_SH_GAMEMOVEMENT_H
#ifdef _WIN32
#pragma once
#endif

#include "gamemovement.h"

class CBasePlayer;

class CFF_SH_GameMovement : public CGameMovement
{
	DECLARE_CLASS(CFF_SH_GameMovement, CGameMovement);
	
public:
	CFF_SH_GameMovement() {};

protected:
	/// Catches all landings
	virtual void	OnLand( float flFallVelocity );

	/// Overwritten from gamemovement.cpp -->
	virtual void	PlayerRoughLandingEffects( float fvol );
	// Ducking
	virtual void	Duck( void );
	virtual void	FinishUnDuck( void );
	/// Overwritten from gamemovement.cpp <---

	/// For debug purposes only
	virtual void	PlayerMove(	void );
};

#endif