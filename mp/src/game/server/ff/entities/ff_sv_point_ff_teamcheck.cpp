#include "cbase.h"
#include "ff_sh_util.h"
#include "ff_sv_point_ff_teamcheck.h"


LINK_ENTITY_TO_CLASS( point_ff_teamcheck, CFF_SV_PointFFTeamCheck );

BEGIN_DATADESC( CFF_SV_PointFFTeamCheck )
	// Keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iTeamsAllowed, FIELD_INTEGER, "teams_allowed" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetTeamsAllowed", InputSetTeamsAllowed ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SwapTeams", InputSwapTeams ),
END_DATADESC()


void CFF_SV_PointFFTeamCheck::InputSetTeamsAllowed( inputdata_t &inputdata )
{
	m_iTeamsAllowed = inputdata.value.Int();
}

void CFF_SV_PointFFTeamCheck::InputSwapTeams( inputdata_t &inputdata )
{
	m_iTeamsAllowed = ~m_iTeamsAllowed;
}

bool CFF_SV_PointFFTeamCheck::IsTeamAllowed(int iTeamNum)
{
	if(!m_iTeamsAllowed)
		return true;

	if(m_iTeamsAllowed & FF_UTIL_GetTeamBit(iTeamNum))
		return true;

	return false;
}
