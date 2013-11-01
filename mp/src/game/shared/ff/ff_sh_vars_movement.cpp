#include "cbase.h"
#include "ff_sh_vars_movement.h"

#ifdef CLIENT_DLL
ConVar cl_jumpqueue( "cl_jumpqueue", "0", FCVAR_ARCHIVE | FCVAR_USERINFO, "Enables jump queue (have to let go and press jump in between concurrent jumps) if set to 1" );
#endif
