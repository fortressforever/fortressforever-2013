#include "cbase.h"
#include "ff_sh_item_ff_goal.h"


LINK_ENTITY_TO_CLASS( item_ff_goal, CFF_SH_ItemFFGoal );

#ifdef GAME_DLL
IMPLEMENT_SERVERCLASS_ST( CFF_SH_ItemFFGoal, DT_FF_ItemFFGoal )
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_SH_ItemFFGoal, DT_FF_ItemFFGoal, CFF_SH_ItemFFGoal )
END_RECV_TABLE()
#endif


#ifdef GAME_DLL
void CFF_SH_ItemFFGoal::Spawn()
{
	BaseClass::Spawn();

	// TODO: Check if needs to drop to floor. Note: This should be done in the info_ versions spawn
	if( false )
		UTIL_DropToFloor( this, MASK_SOLID );

	m_vecSpawnOrigin = GetAbsOrigin();
}
#endif

CBasePlayer* CFF_SH_ItemFFGoal::GetCarrier()
{
	return m_hItemCarrier;
}

const Vector& CFF_SH_ItemFFGoal::GetSpawnOrigin()
{
	return m_vecSpawnOrigin;
}
