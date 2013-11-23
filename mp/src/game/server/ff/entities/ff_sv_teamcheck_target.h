#pragma once

#ifndef FF_SV_TEAMCHECK_TARGET_H
#define FF_SV_TEAMCHECK_TARGET_H

#include "ff_sv_env_ff_message.h"


class CFF_SV_TeamcheckTarget : public CFF_SV_EnvFFMessage
{
public:
	DECLARE_CLASS( CFF_SV_TeamcheckTarget, CFF_SV_EnvFFMessage );
	DECLARE_DATADESC();

	CFF_SV_TeamcheckTarget()
	{
	}

	bool IsTeamAllowed(int iTeamNum);

private:
	string_t m_iszTeamTarget;
};


#endif // FF_SV_TEAMCHECK_TARGET_H
