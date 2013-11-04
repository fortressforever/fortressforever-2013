#include "cbase.h"
#include "ff_sh_team_manager.h"

#ifdef GAME_DLL
#include "entitylist.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef GAME_DLL

static void ClassRestrictionChange( IConVar *var, const char *pOldString, float fOldVal );

// we gotta do this anyway for other classes to use em directly without having to lookup
ConVar	cr_scout( "cr_scout", "0", 0, "Max number of scouts", ClassRestrictionChange );
ConVar	cr_sniper( "cr_sniper",	"0", 0, "Max number of snipers", ClassRestrictionChange );
ConVar	cr_soldier( "cr_soldier", "0", 0, "Max number of soldiers", ClassRestrictionChange );
ConVar	cr_demoman( "cr_demoman", "0", 0, "Max number of demoman", ClassRestrictionChange );
ConVar	cr_medic( "cr_medic", "0", 0, "Max number of medic", ClassRestrictionChange );
ConVar	cr_hwguy( "cr_hwguy", "0", 0, "Max number of hwguy", ClassRestrictionChange );
ConVar	cr_pyro( "cr_pyro",	"0", 0, "Max number of pyro", ClassRestrictionChange );
ConVar	cr_spy( "cr_spy", "0", 0, "Max number of spy", ClassRestrictionChange );
ConVar	cr_engineer( "cr_engineer",	"0", 0, "Max number of engineer", ClassRestrictionChange );
ConVar	cr_civilian( "cr_civilian",	"0", 0, "Max number of engineer", ClassRestrictionChange );

//static CUtlDict<const char*, int> classCvarToIndexDict;
// do this so we can find correct idx to pass to team manager
// (from old CFFTeam::UpdateTeamLimits indices)
static ConVar classRestrictionCvars[] =
{
	cr_scout,
	cr_sniper,
	cr_soldier,
	cr_demoman,
	cr_medic,
	cr_hwguy,
	cr_pyro,
	cr_spy,
	cr_engineer,
	cr_civilian,
};

// Need to update the real class limits for this map
static void ClassRestrictionChange( IConVar *var, const char *pOldString, float fOldVal )
{
	// Update the team limits (skip unassigned and spec, they dont need limits applied )
	for ( int i = TEAM_SPECTATOR + 1; i < g_Teams.Count(); i++ )
	{
		CFF_SH_TeamManager *pTeam = GetGlobalFFTeam( i );
		if ( !pTeam )
			return;

		ConVar *conVar = static_cast<ConVar*>(var);
		if ( !conVar )
			return;

		int idx = -1;
		for ( int i = 0; i < CLASS_COUNT; i ++)
		{
			if ( FStrEq( conVar->GetName(), classRestrictionCvars[i].GetName() ) )
			{
				idx = i;
				break;
			}
		}
		
		if ( idx == -1  )
			return;
		pTeam->UpdateClassLimit( idx );
	}
}

#endif // GAME_DLL


LINK_ENTITY_TO_CLASS( ff_team_manager, CFF_SH_TeamManager );

#ifdef GAME_DLL
//Missing RecvProp for
IMPLEMENT_SERVERCLASS_ST( CFF_SH_TeamManager, DT_FFTeamManager )
	SendPropInt( SENDINFO( m_iAllies ) ), 
	SendPropInt( SENDINFO( m_iFortPoints ) ),
	SendPropArray3( SENDINFO_ARRAY3(m_iClasses), SendPropInt( SENDINFO_ARRAY(m_iClasses), 4 ) ),
	SendPropInt( SENDINFO( m_iMaxPlayers ) ),
	//SendPropString( SENDINFO( m_szTeamname ) ), already done in base data table
	SendPropInt( SENDINFO( m_iDeaths ) ),
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_CL_TeamManager, DT_FFTeamManager, CFF_SV_TeamManager )
	RecvPropInt( RECVINFO( m_iAllies ) ), 
	RecvPropInt( RECVINFO( m_iFortPoints ) ),
	RecvPropArray3( RECVINFO_ARRAY(m_iClasses), RecvPropInt( RECVINFO(m_iClasses[0]))),
	RecvPropInt( RECVINFO( m_iMaxPlayers ) ),
	//RecvPropString( RECVINFO( m_szTeamname ) ), already done in base data table
	RecvPropInt( RECVINFO( m_iDeaths ) ),
END_RECV_TABLE()
#endif

CFF_SH_TeamManager *GetGlobalFFTeam( int iIndex ) 
{
	return (CFF_SH_TeamManager*) GetGlobalTeam( iIndex );
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: Needed because this is an entity, but should never be used
//-----------------------------------------------------------------------------
void CFF_SH_TeamManager::Init( const char *pName, int iNumber )
{
	BaseClass::Init( pName, iNumber );
	NetworkProp()->SetUpdateInterval( 0.75f );
	memset( &m_iClasses, -1, sizeof( m_iClasses ) );	
	m_iAllies = 0;										
}

void CFF_SH_TeamManager::AddDeaths( int iDeaths )
{
	m_iDeaths += iDeaths;
}

void CFF_SH_TeamManager::AddFortPoints( int iFortPoints )
{
	m_iFortPoints += iFortPoints;
}

void CFF_SH_TeamManager::ClearAllies()
{
	m_iAllies = 0;
}

void CFF_SH_TeamManager::SetAllies( int iTeam )
{
	//m_iAllies = iAllies;
	m_iAllies |= 1 << iTeam;
}

void CFF_SH_TeamManager::SetClassLimit( int iClass, int iLimit )
{
	// no change needed
	if ( m_iClassesMap[iClass] == iLimit )
		return;
	
	m_iClassesMap[iClass] = iLimit;
	UpdateClassLimit( iClass );
}

void CFF_SH_TeamManager::UpdateClassLimit( int iClassIdx )
{
	// if the map or cvar is 0 it will always use the other
	int curCvar = classRestrictionCvars[iClassIdx].GetInt();
	int curMap = m_iClassesMap[iClassIdx];
	int newVal = min ( curCvar == 0 ? curMap : curCvar, curMap == 0 ? curCvar : curMap);
	m_iClasses.Set( iClassIdx, newVal );

	DevMsg("CFF_SH_TeamManager::UpdateLimit: set class idx %i limit to %i\n", iClassIdx, newVal );
}

void CFF_SH_TeamManager::UpdateAllClassLimits( void )
{
	for ( int i = 0; i < ARRAYSIZE(m_iClassesMap); i++ )
		UpdateClassLimit( i );
}

void CFF_SH_TeamManager::SetDeaths( int iDeaths )
{
	m_iDeaths = iDeaths;
}

void CFF_SH_TeamManager::SetName( const char *pName )
{
	if (!pName)
		return;

	Q_strncpy( m_szTeamname.GetForModify(), pName, MAX_TEAM_NAME_LENGTH );
}

void CFF_SH_TeamManager::SetFortPoints( int iFortPoints )
{
	m_iFortPoints = iFortPoints;
}

void CFF_SH_TeamManager::SetTeamLimits( int iTeamLimits )
{
	m_iMaxPlayers = iTeamLimits;
}

#endif // GAME_DLL

int CFF_SH_TeamManager::GetAllies( void )
{
	return m_iAllies;
}

int CFF_SH_TeamManager::GetClassLimit ( int iClass )
{
	if ( iClass < 0 || iClass > CLASS_COUNT )
		return -1;
#ifdef CLIENT_DLL
	return m_iClasses[iClass];
#else
	return m_iClasses.Get(iClass);
#endif
}

int CFF_SH_TeamManager::GetFortPoints( void ) 
{
	return m_iFortPoints;
}

const char *CFF_SH_TeamManager::GetName( void ) 
{
#ifdef CLIENT_DLL
	return m_szTeamname;
#else
	return m_szTeamname.Get();
#endif
}

int CFF_SH_TeamManager::GetTeamLimits( void )
{
	return m_iMaxPlayers;
}

#ifdef GAME_DLL
bool CFF_SH_TeamManager::HandlePlayerTeamCommand( CFF_SV_Player &pPlayer, const char *pTeamName )
{
	if ( !pTeamName )
		return false;

	int iOldTeam = pPlayer.GetTeamNumber();
	int iTeam = TEAM_UNASSIGNED;

	if ( FStrEq( pTeamName, "auto" ) )
	{
		iTeam = PickAutoJoinTeam( );
	}
	else if ( FStrEq( pTeamName, "spec" ) )
	{
		iTeam = TEAM_SPECTATOR;
	}
	else if ( FStrEq( pTeamName, "red" ) ) 
	{
		iTeam = TEAM_RED;
	}
	else if ( FStrEq( pTeamName, "blue" ) ) 
	{
		iTeam = TEAM_BLUE;
	}
	else if ( FStrEq( pTeamName, "yellow" ) ) 
	{
		iTeam = TEAM_YELLOW;
	}
	else if ( FStrEq( pTeamName, "green" ) ) 
	{
		iTeam = TEAM_GREEN;
	}
	else if ( FStrEq( pTeamName, "custom1" ) ) 
	{
		iTeam = TEAM_CUSTOM1;
	}
	else if ( FStrEq( pTeamName, "custom2" ) ) 
	{
		iTeam = TEAM_CUSTOM2;
	}
	else if ( FStrEq( pTeamName, "custom3" ) ) 
	{
		iTeam = TEAM_CUSTOM3;
	}
	else if ( FStrEq( pTeamName, "custom4" ) ) 
	{
		iTeam = TEAM_CUSTOM4;
	}
	else if ( FStrEq( pTeamName, "custom5" ) ) 
	{
		iTeam = TEAM_CUSTOM5;
	}
	else if ( FStrEq( pTeamName, "custom6" ) ) 
	{
		iTeam = TEAM_CUSTOM6;
	}
	else if ( FStrEq( pTeamName, "custom7" ) ) 
	{
		iTeam = TEAM_CUSTOM7;
	}
	else if ( FStrEq( pTeamName, "custom8" ) ) 
	{
		iTeam = TEAM_CUSTOM8;
	}
	else 
	{
		// no known hardcoded team name, 
		// try to find team by current team names just for the hell of it
		for ( int i = 0; i < g_Teams.Count(); ++i )
		{
			if (!g_Teams[i])
				continue;
			if ( FStrEq( g_Teams[i]->GetName(), pTeamName ) )
			{
				iTeam = g_Teams[i]->GetTeamNumber( );
				break;
			}
		}
	}

	if ( iTeam == TEAM_UNASSIGNED )
	{
		// TODO: say nothin' found poor sap
		return false;
	}

	// check stupid stuff first
	if ( iOldTeam == iTeam )
	{
		// wants to join same team
		return false;
	}

	CFF_SH_TeamManager *pTeam = GetGlobalFFTeam( iTeam );
	if ( !pTeam )
	{
		// non active team or something fucked
		return false;
	}

	// make sure isnt full 
	if ( pTeam->IsTeamFull() )
	{
		return false;
	}

	// TODO: Lua player_switchteam predicate here

	// refactored this, so each step of the process is seperate chunks,
	// since so much per-class state is handled in each
	pPlayer.PreChangeTeam( iOldTeam, iTeam );
	pPlayer.ChangeTeam( iTeam );
	pPlayer.PostChangeTeam( iOldTeam, iTeam );
	return true;
}

// fun fact, on ep1 code this was called UTIL_PickRandomTeam, but it wasnt random, it was auto join
int CFF_SH_TeamManager::PickAutoJoinTeam( )
{
	// TODO:
	return TEAM_BLUE;
}

bool CFF_SH_TeamManager::IsTeamFull() const
{
	if (m_iMaxPlayers == 0)
		return false;

	int numActive = 0;	
	for( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CFF_SV_Player *pPlayer = (CFF_SV_Player *) UTIL_PlayerByIndex( i );
		if( pPlayer && pPlayer->GetTeamNumber( ) == GetTeamNumber( ) )
			numActive++;
	}

	return numActive > m_iMaxPlayers;
}

#endif

//ConCommand ff_team( "ff_team",
//ConCommand ff_team( "ffdbg_dump_teams",
#if defined (_DEBUG) && defined (GAME_DLL)
void DebugSetTeamName_f( const CCommand &args ) 
{
	if ( args.ArgC() < 2 )
		return;

	int teamNum = Q_atoi(args.Arg(1));
	const char* newName = args.Arg(2);
	if ( teamNum > TEAM_COUNT )
		return;
	
	CFF_SH_TeamManager *pTeam = GetGlobalFFTeam( teamNum );
	if ( !pTeam )
		return;
	DevMsg("Current team name ='%s'\n", pTeam->GetName());
	DevMsg("Setting team name to '%s'..\n", newName);
	pTeam->SetName( newName );
	DevMsg("Team name after set ='%s'\n", pTeam->GetName());
}
ConCommand cc_ffdbg_setteamname("ffdbg_set_team_name",  DebugSetTeamName_f);
#endif


