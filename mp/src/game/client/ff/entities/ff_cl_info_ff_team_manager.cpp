#include "cbase.h"
#include "ff_cl_info_ff_team_manager.h"


LINK_ENTITY_TO_CLASS( info_ff_team_manager, CFF_CL_InfoFFTeamManager );

IMPLEMENT_CLIENTCLASS_DT(CFF_CL_InfoFFTeamManager, DT_FF_InfoFFTeamManager, CFF_SV_InfoFFTeamManager)
	RecvPropInt( RECVINFO( m_iAllies ) ), 
	RecvPropInt( RECVINFO( m_iMaxPlayers ) ),
	RecvPropInt( RECVINFO( m_clrTeamColor ) ),
	RecvPropArray3( RECVINFO_ARRAY(m_iClassLimits), RecvPropInt( RECVINFO( m_iClassLimits[0] ) ) ),
END_RECV_TABLE()
