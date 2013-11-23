#pragma once

#ifndef FF_SV_INFO_FF_TEAM_MANAGER_H
#define FF_SV_INFO_FF_TEAM_MANAGER_H

#include "team.h"
#include "ff_sh_shareddefs.h"
#include "ff_sv_player.h"


class CFF_SV_InfoFFTeamManager : public CTeam
{
public:
	DECLARE_CLASS( CFF_SV_InfoFFTeamManager, CTeam );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CFF_SV_InfoFFTeamManager()
	{
		m_iAllies = 0;
		m_iTeamNum = 0;
	}

	void	Spawn( void );
	void	SetTeamName( const char *pTeamName );
	bool	IsTeamFull( void );

	CNetworkVar( int, m_iAllies );
	CNetworkArray( int, m_iClassLimits, FF_CLASS_COUNT );
	CNetworkVar( int, m_iMaxPlayers );
	CNetworkColor32( m_clrTeamColor );

	string_t	m_iszTeamName;
	string_t	m_iszAllies;

	static bool	TryChangeTeam( CFF_SV_Player &pPlayer, int iNewTeam );
	static int	GetAutoAssignTeam();
};


#endif // FF_SV_INFO_FF_TEAM_MANAGER_H
