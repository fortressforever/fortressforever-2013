#include "cbase.h"
#include "ff_sh_vars_movement.h"

#ifdef CLIENT_DLL
ConVar cl_jumpqueue( "cl_jumpqueue", "0", FCVAR_ARCHIVE | FCVAR_USERINFO, "Enables jump queue (have to let go and press jump in between concurrent jumps) if set to 1" );
#endif

ConVar ffdev_tfc_crouch_airmovement( "ffdev_tfc_crouch_airmovement", "0", FCVAR_REPLICATED, "Enables/disables 'slower' movement while in the air; in TFC, crouching cuts your desired movement in 1/3 even in the air" );