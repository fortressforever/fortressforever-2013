#include "cbase.h"
#include "ff_sh_trigger_ff_goal.h"


LINK_ENTITY_TO_CLASS( trigger_ff_goal, CFF_SH_TriggerFFGoal );


#ifdef GAME_DLL
BEGIN_DATADESC( CFF_SH_TriggerFFGoal )
	// Goal touch functions.
	DEFINE_FUNCTION( OnTouching ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SH_TriggerFFGoal, DT_FF_TriggerFFGoal )
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_SH_TriggerFFGoal, DT_FF_TriggerFFGoal, CFF_SH_TriggerFFGoal )
END_RECV_TABLE()
#endif


void CFF_SH_TriggerFFGoal::Spawn()
{
	BaseClass::Spawn();

	SetSolid( SOLID_BSP );
	SetSolidFlags( FSOLID_NOT_SOLID | FSOLID_TRIGGER );

#ifdef GAME_DLL
	SetTouch( &CFF_SH_TriggerFFGoal::OnTouching );
#endif
}


#ifdef GAME_DLL
void CFF_SH_TriggerFFGoal::OnTouching( CBaseEntity *pOther )
{
	BaseClass::OnTouching( pOther );
}
#endif
