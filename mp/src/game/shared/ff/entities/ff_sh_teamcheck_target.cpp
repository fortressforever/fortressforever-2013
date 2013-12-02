#include "cbase.h"
#include "ff_sh_teamcheck_target.h"


#ifdef GAME_DLL
BEGIN_DATADESC( CFF_SH_TeamcheckTarget )
	// Keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iszTeamTarget, FIELD_STRING, "teamcheck_target" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SH_TeamcheckTarget, DT_FF_TeamcheckTarget )
	SendPropEHandle( SENDINFO( m_hTeamTarget ) ),
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_SH_TeamcheckTarget, DT_FF_TeamcheckTarget, CFF_SH_TeamcheckTarget )
	RecvPropEHandle( RECVINFO( m_hTeamTarget ) ),
END_RECV_TABLE()
#endif


#ifdef GAME_DLL
void CFF_SH_TeamcheckTarget::Activate()
{
	BaseClass::Activate();

	if( m_iszTeamTarget != NULL_STRING )
		m_hTeamTarget = dynamic_cast<CFF_SH_InfoFFTeamCheck *>( gEntList.FindEntityByName( NULL, m_iszTeamTarget ) );
}
#endif

bool CFF_SH_TeamcheckTarget::TeamCheck_IsUsed()
{
	if( !m_hTeamTarget )
		return false;

	return true;
}

bool CFF_SH_TeamcheckTarget::TeamCheck_IsTeamAllowed( int iTeamNum )
{
	if( !m_hTeamTarget )
		return false;

	if( m_hTeamTarget->IsTeamAllowed( iTeamNum ) )
		return true;

	return false;
}
