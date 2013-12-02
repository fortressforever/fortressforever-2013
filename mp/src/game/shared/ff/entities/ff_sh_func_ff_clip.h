#pragma once

#ifndef FF_SH_FUNC_FF_CLIP_H
#define FF_SH_FUNC_FF_CLIP_H


#include "ff_sh_trigger_ff_goal.h"


class CFF_SH_FuncFFClip : public CFF_SH_TriggerFFGoal
{
public:
	DECLARE_CLASS( CFF_SH_FuncFFClip, CFF_SH_TriggerFFGoal );

#ifdef GAME_DLL
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_FuncFFClip() {}

	virtual void	Spawn();
	bool	ShouldClipActivator( const CBaseEntity *pActivator );

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


#endif // FF_SH_FUNC_FF_CLIP_H
