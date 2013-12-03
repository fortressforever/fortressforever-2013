#include "cbase.h"
#include "ff_sh_func_ff_clip.h"


LINK_ENTITY_TO_CLASS( func_ff_clip, CFF_SH_FuncFFClip );


#ifdef GAME_DLL
BEGIN_DATADESC( CFF_SH_FuncFFClip )
	// Goal touch functions.
	DEFINE_FUNCTION( OnTouching ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SH_FuncFFClip, DT_FF_FuncFFClip )
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_SH_FuncFFClip, DT_FF_FuncFFClip, CFF_SH_FuncFFClip )
END_RECV_TABLE()
#endif


void CFF_SH_FuncFFClip::Spawn()
{
	BaseClass::Spawn();

	SetSolid( SOLID_BSP );
	SetSolidFlags( FSOLID_TRIGGER ); // Note: Do not add FSOLID_NOT_SOLID. CTraceFilterSimple::ShouldHitEntity() handles collisions for this entity.

#ifdef GAME_DLL
	SetTouch( &CFF_SH_FuncFFClip::OnTouching );
#endif
}

bool CFF_SH_FuncFFClip::ShouldClipActivator( const CBaseEntity *pActivator )
{
	if( !IsGoalEnabled() )
		return false;

	if( PassesCriteriaCheck( pActivator, true, true ) )
		return false;

	return true;
}

#ifdef GAME_DLL
void CFF_SH_FuncFFClip::OnTouching( CBaseEntity *pOther )
{
	BaseClass::OnTouching( pOther );
}
#endif
