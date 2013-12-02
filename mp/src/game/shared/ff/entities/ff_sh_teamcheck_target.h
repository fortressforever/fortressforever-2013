#pragma once

#ifndef FF_SH_TEAMCHECK_TARGET_H
#define FF_SH_TEAMCHECK_TARGET_H


#include "ff_sh_info_ff_teamcheck.h"

#ifdef CLIENT_DLL
#define CFF_SV_EnvFFMessage C_BaseEntity
#else
#include "ff_sv_env_ff_message.h"
#endif

class CFF_SH_TeamcheckTarget : public CFF_SV_EnvFFMessage
{
public:
	DECLARE_CLASS( CFF_SH_TeamcheckTarget, CFF_SV_EnvFFMessage );

#ifdef GAME_DLL
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_TeamcheckTarget()
	{
	}

#ifdef GAME_DLL
	void	Activate( void );
#endif

	bool	TeamCheck_IsUsed( void );
	bool	TeamCheck_IsTeamAllowed( int iTeamNum );

	CNetworkHandle( CFF_SH_InfoFFTeamCheck, m_hTeamTarget );

private:

#ifdef GAME_DLL
	string_t	m_iszTeamTarget;
#endif
};


#endif // FF_SH_TEAMCHECK_TARGET_H
