#pragma once

#ifndef FF_SH_ITEM_FF_GOAL_H
#define FF_SH_ITEM_FF_GOAL_H


#include "ff_sh_info_ff_goal.h"


class CFF_SH_ItemFFGoal : public CFF_SH_InfoFFGoal
{
public:
	DECLARE_CLASS( CFF_SH_ItemFFGoal, CFF_SH_InfoFFGoal );

#ifdef GAME_DLL
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_ItemFFGoal() {}

	CBasePlayer*	GetCarrier( void );
	const Vector&	GetSpawnOrigin( void );

	void			Spawn( void );

#ifdef GAME_DLL
	virtual void	OnTouching( CBaseEntity *pOther );
	virtual void	ActivateGoalStart( CBaseEntity *pActivator );
	virtual void	ActivateGoal( void );
	virtual void	DeactivateGoal( void );

	int UpdateTransmitState( void )
	{
		// The goal must always be transmitted because client side relys on knowing about each goal.
		// We don't set this in the base_goal because so many other entities are derived from it.
		return SetTransmitState( FL_EDICT_ALWAYS );
	}
#endif

private:
	CNetworkHandle( CBasePlayer, m_hItemCarrier );
	CNetworkVector( m_vecSpawnOrigin );
};


#endif // FF_SH_ITEM_FF_GOAL_H
