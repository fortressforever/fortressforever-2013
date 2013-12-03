#pragma once

#ifndef FF_SH_INFO_FF_GOAL_H
#define FF_SH_INFO_FF_GOAL_H


#ifdef GAME_DLL
#include "baseanimating.h"
#else
#include "c_baseanimating.h"
#endif


class CFF_SH_InfoFFGoal : public CBaseAnimating
{
public:
	DECLARE_CLASS( CFF_SH_InfoFFGoal, CBaseAnimating );

#ifdef GAME_DLL
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_InfoFFGoal() {}

	enum GOAL_SPAWNFLAGS
	{
		GSF_DROP_TO_GROUND_ON_SPAWN	= (1<<7),
	};

	virtual void	Precache( void );
	virtual void	Spawn( void );

#ifdef GAME_DLL
	virtual void	Activate( void );

	virtual void	OnTouching( CBaseEntity *pOther );
	virtual void	ActivateGoal( void );
	virtual void	DeactivateGoal( void );

	int UpdateTransmitState( void )
	{
		// The goal must always be transmitted because client side relys on knowing about each goal.
		// We don't set this in the base_goal because so many other entities are derived from it.
		return SetTransmitState( FL_EDICT_ALWAYS );
	}

protected:
	virtual void	Input_Enable( inputdata_t &inputdata );
	virtual void	Input_Disable( inputdata_t &inputdata );
#endif
};


#endif // FF_SH_INFO_FF_GOAL_H
