#include "cbase.h"
#include "ff_sh_util.h"
#include "ff_sh_info_ff_teamcheck.h"


LINK_ENTITY_TO_CLASS( info_ff_teamcheck, CFF_SH_InfoFFTeamCheck );

#ifdef GAME_DLL
BEGIN_DATADESC( CFF_SH_InfoFFTeamCheck )
	// Keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iTeamsAllowed, FIELD_INTEGER, "teams_allowed" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetTeamsAllowed", Input_SetTeamsAllowed ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SwapTeams", Input_SwapTeams ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST_NOBASE( CFF_SH_InfoFFTeamCheck, DT_FF_InfoFFTeamCheck )
	SendPropInt( SENDINFO(m_iTeamsAllowed) ),
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT_NOBASE( CFF_SH_InfoFFTeamCheck, DT_FF_InfoFFTeamCheck, CFF_SH_InfoFFTeamCheck )
	RecvPropInt( RECVINFO(m_iTeamsAllowed) ),
END_RECV_TABLE()
#endif


bool CFF_SH_InfoFFTeamCheck::IsTeamAllowed( int iTeamNum )
{
	if( !m_iTeamsAllowed )
		return true;

	if( m_iTeamsAllowed & FF_UTIL_GetTeamBit(iTeamNum) )
		return true;

	return false;
}

#ifdef GAME_DLL
int CFF_SH_InfoFFTeamCheck::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

void CFF_SH_InfoFFTeamCheck::Input_SetTeamsAllowed( inputdata_t &inputdata )
{
	m_iTeamsAllowed.Set( inputdata.value.Int() );
}

void CFF_SH_InfoFFTeamCheck::Input_SwapTeams( inputdata_t &inputdata )
{
	m_iTeamsAllowed.Set( ~m_iTeamsAllowed.Get() );
}
#endif
