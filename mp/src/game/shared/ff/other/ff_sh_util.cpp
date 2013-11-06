#include "cbase.h"
#include "ff_sh_util.h"
#include "ff_sh_shareddefs.h"


int FF_UTIL_GetTeamBit( int teamNum )
{
	if( teamNum >= sizeof(FF_TEAM_BITS) || teamNum < 0 )
		return 0;

	return FF_TEAM_BITS[teamNum];
}
