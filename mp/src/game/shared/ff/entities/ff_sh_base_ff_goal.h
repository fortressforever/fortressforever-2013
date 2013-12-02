#pragma once

#ifndef FF_SH_BASE_FF_GOAL_H
#define FF_SH_BASE_FF_GOAL_H


#include "ff_sh_teamcheck_target.h"


#define MAX_GOALS_PER_CRITERIA 20
class CFF_SH_ItemFFGoal;

class CFF_SH_BaseFFGoal : public CFF_SH_TeamcheckTarget
{
public:
	DECLARE_CLASS( CFF_SH_BaseFFGoal, CFF_SH_TeamcheckTarget );

#ifdef GAME_DLL
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
#else
	DECLARE_CLIENTCLASS();
#endif

	CFF_SH_BaseFFGoal()
	{
		SetThink( NULL );
		m_bGoalActivated = false;

#ifdef GAME_DLL
		m_clrFailedCriteriaColor.r = 4;
		m_clrFailedCriteriaColor.g = 235;
		m_clrFailedCriteriaColor.b = 4;
		m_clrFailedCriteriaColor.a = 255;
#endif
	}

#ifdef GAME_DLL
	virtual void		Precache( void );
	virtual void		Spawn( void );
	virtual void		Activate( void );
	void				ParseCriteriaKeyValues( void );

	virtual void		OnTouching( CBaseEntity *pOther ); // NOTE: This function is called from its derived classes SetTouch() function (BaseClass::OnTouching()).
	void				TryActivateGoal( CBaseEntity *pActivator );
	void				ActivateGoalStart( CBaseEntity *pActivator );
	void				ActivateGoal( void );
	void				DeactivateGoal( void );
#endif

public:
	enum GOAL_SPAWNFLAGS
	{
		GSF_START_ACTIVE						= (1<<0),
		GSF_START_ENABLED						= (1<<1),
		GSF_ACTIVATED_BY_PLAYER_TOUCH			= (1<<2),
		GSF_LOOP_ACTIVATION_UNTIL_DISABLED		= (1<<3),
		GSF_CHECK_PLAYER_CRITERIA_INDIVIDUALLY	= (1<<4),
	};

	bool			IsGoalEnabled( void );
	bool			IsGoalActivated( void );

protected:
	bool			PassesCriteriaCheck( const CBaseEntity *pEnt, bool bIsActivator=true );

private:
	bool			HandleCriteriaCheckResult( const CBaseEntity *pEnt, bool bIsActivator, bool bReturnValue );

	// Goal varaibles.
	CNetworkVar( bool, m_bGoalEnabled );
	CNetworkVar( bool, m_bGoalActivated );
	CNetworkHandle( CBaseEntity, m_hGoalActivator );

	float	m_fActiveDelay;	// How long to wait before going in the active state.
	float	m_fActiveTime;	// How long to stay in the active state.

	// Criteria check variables.
	CNetworkVar( int, m_iCriteria_TeamsAllowed );
	CNetworkVar( int, m_iCriteria_ClassesAllowed );

	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsCarriedByActivator;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsNotCarriedByActivator;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsCarriedByTeam;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsNotCarriedByTeam;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsCarriedByAny;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsNotCarriedByAny;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsCarriedByOne;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsNotCarriedByOne;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsAtSpawnPoint;
	CUtlVector<CHandle<CFF_SH_ItemFFGoal>>		m_Criteria_ItemsNotAtSpawnPoint;

	CUtlVector<CHandle<CFF_SH_BaseFFGoal>>		m_Criteria_GoalsAreActive;
	CUtlVector<CHandle<CFF_SH_BaseFFGoal>>		m_Criteria_GoalsAreInactive;
	CUtlVector<CHandle<CFF_SH_BaseFFGoal>>		m_Criteria_GoalsAreEnabled;
	CUtlVector<CHandle<CFF_SH_BaseFFGoal>>		m_Criteria_GoalsAreDisabled;

#ifdef GAME_DLL
private:
	void		Input_Enable( inputdata_t &inputdata );
	void		Input_Disable( inputdata_t &inputdata );
	void		Input_Activate( inputdata_t &inputdata );
	void		Input_Inactivate( inputdata_t &inputdata );

	COutputEvent	m_Output_OnEnabled;
	COutputEvent	m_Output_OnDisabled;
	COutputEvent	m_Output_OnActive;
	COutputEvent	m_Output_OnInactive;

	string_t	m_iszActivationSound;
	string_t	m_iszResetSound;

	bool		m_bGoalActive;

	color32		m_clrFailedCriteriaColor;
	float		m_fNextFailedCriteriaMessage[MAX_PLAYERS+1];
	string_t	m_iszFailedCriteriaMessage;

	// NOTE: The strings below are only used for parsing the data from the map keyfields.
	string_t	m_iszCriteria_TeamsAllowed;
	string_t	m_iszCriteria_ClassesAllowed;

	string_t	m_iszCriteria_ItemsCarriedByActivator;
	string_t	m_iszCriteria_ItemsNotCarriedByActivator;
	string_t	m_iszCriteria_ItemsCarriedByTeam;
	string_t	m_iszCriteria_ItemsNotCarriedByTeam;
	string_t	m_iszCriteria_ItemsCarriedByAny;
	string_t	m_iszCriteria_ItemsNotCarriedByAny;
	string_t	m_iszCriteria_ItemsCarriedByOne;
	string_t	m_iszCriteria_ItemsNotCarriedByOne;
	string_t	m_iszCriteria_ItemsAtSpawnPoint;
	string_t	m_iszCriteria_ItemsNotAtSpawnPoint;

	string_t	m_iszCriteria_GoalsAreActive;
	string_t	m_iszCriteria_GoalsAreInactive;
	string_t	m_iszCriteria_GoalsAreEnabled;
	string_t	m_iszCriteria_GoalsAreDisabled;
#endif
};


#endif // FF_SH_BASE_FF_GOAL_H
