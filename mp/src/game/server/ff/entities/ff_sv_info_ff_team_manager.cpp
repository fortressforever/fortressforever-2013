#include "cbase.h"
#include "ff_sv_info_ff_team_manager.h"


LINK_ENTITY_TO_CLASS( info_ff_team_manager, CFF_SV_InfoFFTeamManager );

BEGIN_DATADESC( CFF_SV_InfoFFTeamManager )
	// Keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iTeamNum, FIELD_INTEGER, "team_num" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszTeamName, FIELD_STRING, "team_name" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iMaxPlayers, FIELD_INTEGER, "maxplayers" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszAllies, FIELD_STRING, "allies" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_clrTeamColor, FIELD_COLOR32, "team_color" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_SCOUT], FIELD_INTEGER, "max_scout" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_SNIPER], FIELD_INTEGER, "max_sniper" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_SOLDIER], FIELD_INTEGER, "max_soldier" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_DEMOMAN], FIELD_INTEGER, "max_demoman" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_MEDIC], FIELD_INTEGER, "max_medic" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_HWGUY], FIELD_INTEGER, "max_hwguy" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_PYRO], FIELD_INTEGER, "max_pyro" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_SPY], FIELD_INTEGER, "max_spy" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_ENGINEER], FIELD_INTEGER, "max_engineer" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iClassLimits[FF_CLASS_CIVILIAN], FIELD_INTEGER, "max_civilian" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SV_InfoFFTeamManager, DT_FF_InfoFFTeamManager )
	SendPropInt( SENDINFO( m_iAllies ) ), 
	SendPropInt( SENDINFO( m_iMaxPlayers ) ),
	SendPropInt( SENDINFO( m_clrTeamColor ) ),
	SendPropArray3( SENDINFO_ARRAY3( m_iClassLimits ), SendPropInt( SENDINFO_ARRAY( m_iClassLimits ) ) ),
END_SEND_TABLE()


void CFF_SV_InfoFFTeamManager::Spawn()
{
	// Increase the team number by 1 since FF_TEAM_ONE actually has the value of 2.
	m_iTeamNum++;

	if( m_iTeamNum < FF_TEAM_ONE || m_iTeamNum > FF_TEAM_LAST )
		return;

	// Make sure this team number doesn't already exist in the teams list.
	for( int i=0; i<g_Teams.Count(); i++ )
	{
		if( g_Teams[i]->GetTeamNumber() == m_iTeamNum )
			return;
	}

	if( m_iszTeamName != NULL_STRING )
		SetTeamName( STRING(m_iszTeamName) );

	CUtlVector<char*> allies;
	Q_SplitString( STRING(m_iszAllies), ",", allies );

	int iAllyTeam;
	for( int i=0; i<allies.Count(); i++ )
	{
		iAllyTeam = Q_atoi(allies[i]) + 1; // Increase the ally team by 1 since FF_TEAM_ONE starts at value 2.
		if( iAllyTeam < FF_TEAM_ONE || iAllyTeam > FF_TEAM_LAST )
			continue;

		m_iAllies |= FF_TEAM_BITS[iAllyTeam];
	}

	BaseClass::Init( STRING(m_iszTeamName), m_iTeamNum );
	g_Teams.AddToTail( this );
}

void CFF_SV_InfoFFTeamManager::SetTeamName( const char *pTeamName )
{
	if( !pTeamName )
		return;

	Q_strncpy( m_szTeamname.GetForModify(), pTeamName, MAX_TEAM_NAME_LENGTH );
}

bool CFF_SV_InfoFFTeamManager::TryChangeTeam( CFF_SV_Player &pPlayer, int iNewTeam )
{
	if( iNewTeam == FF_TEAM_AUTO_ASSIGN )
		iNewTeam = GetAutoAssignTeam();

	if ( iNewTeam < FF_TEAM_SPECTATE || iNewTeam > FF_TEAM_LAST )
		return false;

	int iCurrentTeam = pPlayer.GetTeamNumber();
	if ( iCurrentTeam == iNewTeam )
		return false;

	// Make sure this team exists.
	CFF_SV_InfoFFTeamManager *pTeam = dynamic_cast<CFF_SV_InfoFFTeamManager *>(GetGlobalTeam( iNewTeam ));
	if( !pTeam )
		return false;

	// Make sure this team isn't full.
	if( pTeam->IsTeamFull() )
		return false;

	// TODO: Lua player_switchteam predicate here.
	// -->

	// refactored this, so each step of the process is seperate chunks,
	// since so much per-class state is handled in each
	pPlayer.PreChangeTeam( iCurrentTeam, iNewTeam );
	pPlayer.ChangeTeam( iNewTeam );
	pPlayer.PostChangeTeam( iCurrentTeam, iNewTeam );
	return true;
}

int CFF_SV_InfoFFTeamManager::GetAutoAssignTeam()
{
	// TODO: Implement the auto assign code.
	return FF_TEAM_ONE;
}

bool CFF_SV_InfoFFTeamManager::IsTeamFull()
{
	// TODO: Implement this.
	// -->

	return false;
}