#pragma once

#ifndef FF_SV_BASE_FF_GOAL_H
#define FF_SV_BASE_FF_GOAL_H

#include "ff_sv_teamcheck_target.h"


class CFF_SV_BaseFFGoal : public CFF_SV_TeamcheckTarget
{
public:
	DECLARE_CLASS( CFF_SV_BaseFFGoal, CFF_SV_TeamcheckTarget );
	DECLARE_DATADESC();

	CFF_SV_BaseFFGoal()
	{
	}

private:
	bool	CanActivateGoal(CBasePlayer *pActivator);
};

#endif // FF_SV_BASE_FF_GOAL_H
