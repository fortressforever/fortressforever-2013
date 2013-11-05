#ifndef FF_SH_TEAM_MANAGER_H
#define FF_SH_TEAM_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "ff_sh_shareddefs.h"

#ifdef GAME_DLL
#include "team.h"
#include "ff_sv_player.h"
#else
#include "c_team.h"
#define CTeam C_Team
#endif


#ifdef CLIENT_DLL
	#define CFF_SH_TeamManager CFF_CL_TeamManager
#else
	#define CFF_SH_TeamManager CFF_SV_TeamManager
#endif

class CFF_SH_TeamManager : public CTeam
{
	DECLARE_CLASS( CFF_SH_TeamManager, CTeam );
#ifdef GAME_DLL
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

public:
// variables
#ifdef GAME_DLL
	int m_iClassesMap[FF_CLASS_COUNT];					// this is just the map limits
	CNetworkVar( int, m_iAllies ); 
	CNetworkVar( int, m_iFortPoints );
	CNetworkArray( int, m_iClasses, FF_CLASS_COUNT );	// this is the actual limit, needed by the client	
	CNetworkVar( int, m_iMaxPlayers );
#else
	int m_iAllies;
	int m_iFortPoints;
	int m_iClasses[FF_CLASS_COUNT];
	int m_iMaxPlayers;
#endif

	// setters / writes
#ifdef GAME_DLL
	// Initialization
	virtual void Init( const char *pName, int iNumber );
	void AddDeaths( int iDeaths );
	void AddFortPoints( int iFortPoints );
	void ClearAllies();
	void SetAllies( int );
	void SetClassLimit( int, int );		// Set the map's class limit
	void SetDeaths( int iDeatsh );
	void SetName( const char *pName );
	void SetFortPoints( int iFortPoints );
	void SetTeamLimits( int );
	void UpdateClassLimit( int idx );
	void UpdateClassLimit( int iClassIdx, int conVarVal );
	void UpdateAllClassLimits( void );

	static bool HandlePlayerTeamCommand( CFF_SV_Player &pPlayer, const char* pTeam );
	static int PickAutoJoinTeam( );

	bool IsTeamFull() const;
#endif // GAME_DLL

	// shared getters
	int GetAllies( void );
	int GetClassLimit( int );
	int GetDeaths( void );	
	int GetFortPoints( void );
	const char *GetName ( void );
	int GetTeamLimits( void );
};

extern CFF_SH_TeamManager *GetGlobalFFTeam( int iIndex );

#endif // FF_SV_TEAM_MANAGER_H