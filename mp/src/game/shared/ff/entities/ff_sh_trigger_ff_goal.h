#pragma once

#ifndef FF_SH_TRIGGER_FF_GOAL_H
#define FF_SH_TRIGGER_FF_GOAL_H


#include "ff_sh_base_ff_goal.h"

#ifdef GAME_DLL
	#include "triggers.h"
#else
	#define CTriggerMultiple CFF_SH_BaseFFGoal
#endif


class CFF_SH_TriggerFFGoal : public CTriggerMultiple
{
public:
	DECLARE_CLASS( CFF_SH_TriggerFFGoal, CTriggerMultiple );

#ifdef GAME_DLL
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_TriggerFFGoal() {}

	virtual void	Spawn();

#ifdef GAME_DLL
	virtual void	OnTouching( CBaseEntity *pOther );

	int UpdateTransmitState( void )
	{
		// The goal must always be transmitted because client side relys on knowing about each goal.
		// We don't set this in the base_goal because so many other entities are derived from it.
		return SetTransmitState( FL_EDICT_ALWAYS );
	}
#endif
};


#endif // FF_SH_TRIGGER_FF_GOAL_H
