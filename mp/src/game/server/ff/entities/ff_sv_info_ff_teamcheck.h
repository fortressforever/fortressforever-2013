#pragma once

#ifndef FF_SV_INFO_FF_TEAMCHECK_H
#define FF_SV_INFO_FF_TEAMCHECK_H


class CFF_SV_InfoFFTeamCheck : public CBaseEntity
{
public:
	DECLARE_CLASS( CFF_SV_InfoFFTeamCheck, CBaseEntity );
	DECLARE_DATADESC();

	CFF_SV_InfoFFTeamCheck()
	{
	}

	bool IsTeamAllowed(int iTeamNum);

private:
	void InputSetTeamsAllowed( inputdata_t &inputdata );
	void InputSwapTeams( inputdata_t &inputdata );

	int m_iTeamsAllowed;
};


#endif // FF_SV_INFO_FF_TEAMCHECK_H
