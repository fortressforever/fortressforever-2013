#pragma once

#ifndef FF_SH_ITEM_FF_GOAL_H
#define FF_SH_ITEM_FF_GOAL_H


#ifdef GAME_DLL
#include "baseanimating.h"
#else
#include "c_baseanimating.h"
#endif


class CFF_SH_ItemFFGoal : public CBaseAnimating	// TODO: Derive from info_ version. Note: info_ should still derive from CBaseAnimating.
{
public:
	DECLARE_CLASS( CFF_SH_ItemFFGoal, CBaseAnimating );	// TODO: Derive from info_ version. Note: info_ should still derive from CBaseAnimating.

#ifdef GAME_DLL
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_ItemFFGoal() {}

	CBasePlayer*	GetCarrier( void );
	const Vector&	GetSpawnOrigin( void );

#ifdef GAME_DLL
	void			Spawn( void );

	int UpdateTransmitState( void )
	{
		// The goal must always be transmitted because client side relys on knowing about each goal.
		// We don't set this in the base_goal because so many other entities are derived from it.
		return SetTransmitState( FL_EDICT_ALWAYS );
	}
#endif

private:
	CHandle<CBasePlayer>	m_hItemCarrier;		// TODO: network this.
	Vector					m_vecSpawnOrigin;	// TODO: network this.
};


#endif // FF_SH_ITEM_FF_GOAL_H
