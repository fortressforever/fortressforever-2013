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
	// --> FF functions
	bool	CanJump( void );
	bool	DoDoubleJump( float &flJumpSpeed );
	bool	DoTrimp( float flGroundDotProduct, float &flSpeed, float &flJumpSpeed );
	bool	DoDownTrimp( float flGroundDotProduct, float &flSpeed, float &flJumpSpeed );
	float	ApplySoftCap( float &flSpeed );
	float	ApplyHardCap( float &flSpeed );
	// <-- FF functions
	
	// --> Extended functions (meaning the BaseClass function is always called)
	virtual void	PlayerMove(	void );
	virtual void	OnLand( float flFallVelocity );
	// <-- Extended functions

	// --> Overwritten functions
	// Jumping
	virtual void	PlayerRoughLandingEffects( float fvol );
	virtual bool	CheckJumpButton( void );

	// Ducking
	virtual void	Duck( void );
	virtual void	FinishUnDuck( void );
	// <--- Overwritten functions
};

#endif