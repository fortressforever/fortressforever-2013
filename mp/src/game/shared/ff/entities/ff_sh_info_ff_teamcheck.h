#pragma once

#ifndef FF_SH_INFO_FF_TEAMCHECK_H
#define FF_SH_INFO_FF_TEAMCHECK_H


class CFF_SH_InfoFFTeamCheck : public CBaseEntity
{
public:
	DECLARE_CLASS( CFF_SH_InfoFFTeamCheck, CBaseEntity );

#ifdef GAME_DLL
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_InfoFFTeamCheck()
	{
	}

	CNetworkVar( int, m_iTeamsAllowed );
	bool	IsTeamAllowed( int iTeamNum );

#ifdef GAME_DLL
	int		UpdateTransmitState( void );

private:
	void	Input_SetTeamsAllowed( inputdata_t &inputdata );
	void	Input_SwapTeams( inputdata_t &inputdata );
#endif
};


#endif // FF_SH_INFO_FF_TEAMCHECK_H
