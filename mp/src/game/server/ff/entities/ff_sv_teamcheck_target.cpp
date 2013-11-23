#include "cbase.h"
#include "ff_sv_info_ff_teamcheck.h"
#include "ff_sv_teamcheck_target.h"


// Don't link this class to an entity. This class is only to be for inheritance.
//LINK_ENTITY_TO_CLASS( teamcheck_target, CFF_SV_TeamcheckTarget );

BEGIN_DATADESC( CFF_SV_TeamcheckTarget )
	// Keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iszTeamTarget, FIELD_STRING, "teamcheck_target" ),
END_DATADESC()


bool CFF_SV_TeamcheckTarget::IsTeamAllowed( int iTeamNum )
{
	if( !m_iszTeamTarget )
		return true;

	CFF_SV_InfoFFTeamCheck *pEnt = dynamic_cast<CFF_SV_InfoFFTeamCheck *>( gEntList.FindEntityByName( NULL, m_iszTeamTarget ) );
	if( !pEnt )
		return true;

	if( pEnt->IsTeamAllowed( iTeamNum ) )
		return true;

	return false;
}
