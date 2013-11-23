#include "cbase.h"
#include "ff_sv_base_ff_goal.h"


// Don't link this class to an entity. This class is only to be for inheritance.
//LINK_ENTITY_TO_CLASS( base_ff_goal, CFF_SV_BaseFFGoal );

BEGIN_DATADESC( CFF_SV_BaseFFGoal )
END_DATADESC()


bool CanActivateGoal(CBasePlayer *pActivator)
{
	// TODO: Check to see if this player needs to be on a certain team to activate this goal.
	// -->

	// TODO: Check to see if this player needs to be a certain class to activate this goal.
	// -->

	// TODO: Check to see if this player needs to own a certain item_ff_goal.
	// -->

	// TODO: Check to see if this player needs to NOT own a certain item_ff_goal.
	// -->

	return true;
}