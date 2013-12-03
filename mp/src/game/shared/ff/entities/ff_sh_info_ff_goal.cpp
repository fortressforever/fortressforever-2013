#include "cbase.h"
#include "ff_sh_info_ff_goal.h"


LINK_ENTITY_TO_CLASS( info_ff_goal, CFF_SH_InfoFFGoal );

#ifdef GAME_DLL
BEGIN_DATADESC( CFF_SH_InfoFFGoal )
	// Goal touch functions.
	DEFINE_FUNCTION( OnTouching ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SH_InfoFFGoal, DT_FF_InfoFFGoal )
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_SH_InfoFFGoal, DT_FF_InfoFFGoal, CFF_SH_InfoFFGoal )
END_RECV_TABLE()
#endif


void CFF_SH_InfoFFGoal::Precache( void )
{
	if( GetModelName() != NULL_STRING )
		PrecacheModel( STRING(GetModelName()) );

	BaseClass::Precache();
}

void CFF_SH_InfoFFGoal::Spawn()
{
	Precache();
	BaseClass::Spawn();

	if( GetModelName() != NULL_STRING )
		SetModel( STRING(GetModelName()) );

	SetSolid( SOLID_BBOX );
	SetSolidFlags( FSOLID_NOT_SOLID | FSOLID_TRIGGER );

#ifdef GAME_DLL
	if( m_spawnflags & GSF_DROP_TO_GROUND_ON_SPAWN )
		UTIL_DropToFloor( this, MASK_SOLID );

	SetTouch( &CFF_SH_InfoFFGoal::OnTouching );
#endif
}

#ifdef GAME_DLL
void CFF_SH_InfoFFGoal::Activate()
{
	BaseClass::Activate();

	if( !IsGoalEnabled() )
		AddEffects( EF_NODRAW );
}

void CFF_SH_InfoFFGoal::OnTouching( CBaseEntity *pOther )
{
	BaseClass::OnTouching( pOther );
}

void CFF_SH_InfoFFGoal::ActivateGoal()
{
	AddEffects( EF_NODRAW );
	BaseClass::ActivateGoal();
}

void CFF_SH_InfoFFGoal::DeactivateGoal()
{
	if( IsGoalEnabled() )
		RemoveEffects( EF_NODRAW );

	BaseClass::DeactivateGoal();
}

void CFF_SH_InfoFFGoal::Input_Enable( inputdata_t &inputdata )
{
	BaseClass::Input_Enable( inputdata );

	if( !IsGoalActivated() )
		RemoveEffects( EF_NODRAW );
}

void CFF_SH_InfoFFGoal::Input_Disable( inputdata_t &inputdata )
{
	BaseClass::Input_Disable( inputdata );
	AddEffects( EF_NODRAW );
}
#endif
