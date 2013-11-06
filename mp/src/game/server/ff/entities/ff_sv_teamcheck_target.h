#pragma once

#ifndef FF_SV_TEAMCHECK_TARGET_H
#define FF_SV_TEAMCHECK_TARGET_H


// NOTE: We might just have to delete this entity and place all the code in CBaseEntity. Unless we find a way to derive from multiple classes.
class CFF_SV_TeamcheckTarget : public CBaseAnimating
{
public:
	DECLARE_CLASS( CFF_SV_TeamcheckTarget, CBaseAnimating );
	DECLARE_DATADESC();

	CFF_SV_TeamcheckTarget()
	{
	}

	bool IsTeamAllowed(int iTeamNum);

private:
	string_t m_iszTeamTarget;
};


#endif // FF_SV_TEAMCHECK_TARGET_H
