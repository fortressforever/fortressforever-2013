#pragma once

#ifndef FF_CL_INFO_FF_TEAM_MANAGER_H
#define FF_CL_INFO_FF_TEAM_MANAGER_H

#include "cbase.h"
#include "c_team.h"
#include "ff_sh_shareddefs.h"


class CFF_CL_InfoFFTeamManager : public C_Team
{
public:
	DECLARE_CLASS( CFF_CL_InfoFFTeamManager, C_Team );
	DECLARE_CLIENTCLASS();

	CFF_CL_InfoFFTeamManager() {}

	int		m_iAllies;
	int		m_iMaxPlayers;
	color32	m_clrTeamColor;
	int		m_iClassLimits[FF_CLASS_COUNT];
};

#endif // FF_CL_INFO_FF_TEAM_MANAGER_H
