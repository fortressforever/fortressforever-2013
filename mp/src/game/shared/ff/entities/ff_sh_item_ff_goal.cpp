#include "cbase.h"
#include "ff_sh_item_ff_goal.h"


LINK_ENTITY_TO_CLASS( item_ff_goal, CFF_SH_ItemFFGoal );

#ifdef GAME_DLL
BEGIN_DATADESC( CFF_SH_ItemFFGoal )
	// Goal touch functions.
	DEFINE_FUNCTION( OnTouching ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SH_ItemFFGoal, DT_FF_ItemFFGoal )
	SendPropEHandle( SENDINFO( m_hItemCarrier ) ),
	SendPropVector( SENDINFO( m_vecSpawnOrigin ) ),
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_SH_ItemFFGoal, DT_FF_ItemFFGoal, CFF_SH_ItemFFGoal )
	RecvPropEHandle( RECVINFO( m_hItemCarrier ) ),
	RecvPropVector( RECVINFO( m_vecSpawnOrigin ) ),
END_RECV_TABLE()
#endif


void CFF_SH_ItemFFGoal::Spawn()
{
	BaseClass::Spawn();
	m_vecSpawnOrigin = GetAbsOrigin();
}

CBasePlayer* CFF_SH_ItemFFGoal::GetCarrier()
{
	return m_hItemCarrier;
}

const Vector& CFF_SH_ItemFFGoal::GetSpawnOrigin()
{
	return m_vecSpawnOrigin;
}

#ifdef GAME_DLL
void CFF_SH_ItemFFGoal::OnTouching( CBaseEntity *pOther )
{
	BaseClass::OnTouching( pOther );
}

void CFF_SH_ItemFFGoal::ActivateGoalStart( CBaseEntity *pActivator )
{
	BaseClass::ActivateGoalStart( pActivator );

	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(pActivator);
	if( !pPlayer )
		return;

	// Parent this item to the activator.
	SetSolid( SOLID_NONE );
	SetParent( pPlayer );

	m_hItemCarrier = pPlayer;
}

void CFF_SH_ItemFFGoal::ActivateGoal()
{
	BaseClass::ActivateGoal();
	RemoveEffects( EF_NODRAW );
}

void CFF_SH_ItemFFGoal::DeactivateGoal()
{
	BaseClass::DeactivateGoal();
}
#endif
