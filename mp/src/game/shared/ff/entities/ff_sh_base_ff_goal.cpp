#include "cbase.h"
#include "ff_sh_base_ff_goal.h"
#include "ff_sh_item_ff_goal.h"
#include "ff_sh_shareddefs.h"

#ifdef GAME_DLL
#include "ff_sv_util.h"
#include "dt_utlvector_send.h"
#else
#include "dt_utlvector_recv.h"
#endif


#ifdef GAME_DLL
BEGIN_DATADESC( CFF_SH_BaseFFGoal )
	// Think functions.
	DEFINE_THINKFUNC( ActivateGoal ),
	DEFINE_THINKFUNC( DeactivateGoal ),

	// Goal inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", Input_Enable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", Input_Disable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Activate", Input_Activate ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Inactivate", Input_Inactivate ),

	// Goal outputs.
	DEFINE_OUTPUT( m_Output_OnEnabled, "OnEnabled" ),
	DEFINE_OUTPUT( m_Output_OnDisabled, "OnDisabled" ),
	DEFINE_OUTPUT( m_Output_OnActive, "OnActive" ),
	DEFINE_OUTPUT( m_Output_OnInactive, "OnInactive" ),

	// Goal keys.
	DEFINE_KEYFIELD_NOT_SAVED( m_fActiveDelay, FIELD_FLOAT, "active_delay" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_fActiveTime, FIELD_FLOAT, "active_time" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszActivationSound, FIELD_SOUNDNAME, "activation_sound" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszResetSound, FIELD_SOUNDNAME, "reset_sound" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszFailedCriteriaMessage, FIELD_STRING, "msg_if_activator_fails_criteria" ),

	// Criteria check keys.
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_TeamsAllowed, FIELD_STRING, "teams_allowed" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ClassesAllowed, FIELD_STRING, "classes_allowed" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsCarriedByActivator, FIELD_STRING, "if_items_carried_by_ap" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsNotCarriedByActivator, FIELD_STRING, "if_items_not_carried_by_ap" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsCarriedByTeam, FIELD_STRING, "if_items_carried_by_team" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsNotCarriedByTeam, FIELD_STRING, "if_items_not_carried_by_team" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsCarriedByAny, FIELD_STRING, "if_items_carried_by_any" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsNotCarriedByAny, FIELD_STRING, "if_items_not_carried_by_any" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsCarriedByOne, FIELD_STRING, "if_items_carried_by_one" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsNotCarriedByOne, FIELD_STRING, "if_items_not_carried_by_one" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsAtSpawnPoint, FIELD_STRING, "if_items_at_spawn_point" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_ItemsNotAtSpawnPoint, FIELD_STRING, "if_items_not_at_spawn_point" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_GoalsAreActive, FIELD_STRING, "if_goals_are_active" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_GoalsAreInactive, FIELD_STRING, "if_goals_are_inactive" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_GoalsAreEnabled, FIELD_STRING, "if_goals_are_enabled" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszCriteria_GoalsAreDisabled, FIELD_STRING, "if_goals_are_disabled" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SH_BaseFFGoal, DT_FF_BaseFFGoal )
	SendPropBool( SENDINFO( m_bGoalEnabled ) ),
	SendPropBool( SENDINFO( m_bGoalActivated ) ),
	SendPropEHandle( SENDINFO( m_hGoalActivator ) ),

	SendPropInt( SENDINFO( m_iCriteria_TeamsAllowed ) ),
	SendPropInt( SENDINFO( m_iCriteria_ClassesAllowed ) ),

	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsCarriedByActivator ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByActivator ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsCarriedByTeam ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByTeam ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsCarriedByAny ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByAny ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsCarriedByOne ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByOne ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsAtSpawnPoint ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_ItemsNotAtSpawnPoint ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),

	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_GoalsAreActive ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_GoalsAreInactive ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_GoalsAreEnabled ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
	SendPropUtlVector( SENDINFO_UTLVECTOR( m_Criteria_GoalsAreDisabled ), MAX_GOALS_PER_CRITERIA, SendPropEHandle( NULL, 0 ) ),
END_SEND_TABLE()
#else
IMPLEMENT_CLIENTCLASS_DT( CFF_SH_BaseFFGoal, DT_FF_BaseFFGoal, CFF_SH_BaseFFGoal )
	RecvPropBool( RECVINFO( m_bGoalEnabled ) ),
	RecvPropBool( RECVINFO( m_bGoalActivated ) ),
	RecvPropEHandle( RECVINFO( m_hGoalActivator ) ),

	RecvPropInt( RECVINFO( m_iCriteria_TeamsAllowed ) ),
	RecvPropInt( RECVINFO( m_iCriteria_ClassesAllowed ) ),

	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsCarriedByActivator ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByActivator ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsCarriedByTeam ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByTeam ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsCarriedByAny ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByAny ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsCarriedByOne ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsNotCarriedByOne ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsAtSpawnPoint ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_ItemsNotAtSpawnPoint ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),

	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_GoalsAreActive ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_GoalsAreInactive ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_GoalsAreEnabled ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
	RecvPropUtlVector( RECVINFO_UTLVECTOR( m_Criteria_GoalsAreDisabled ), MAX_GOALS_PER_CRITERIA, RecvPropEHandle( NULL, 0, 0 ) ),
END_RECV_TABLE()
#endif


bool CFF_SH_BaseFFGoal::IsGoalEnabled()
{
	return m_bGoalEnabled;
}

bool CFF_SH_BaseFFGoal::IsGoalActivated()
{
	return m_bGoalActivated;
}

bool CFF_SH_BaseFFGoal::HandleCriteriaCheckResult( const CBaseEntity *pEnt, bool bIsActivator, bool bReturnValue )
{
#ifdef GAME_DLL
	if( bIsActivator && !bReturnValue && m_iszFailedCriteriaMessage != NULL_STRING )
	{
		// Display criteria failed message to the activator.
		CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>( Instance(pEnt->edict()) );
		if( pPlayer )
		{
			int iPlayerIndex = ENTINDEX(pPlayer);
			if( m_fNextFailedCriteriaMessage[iPlayerIndex] <= gpGlobals->curtime )
			{
				FF_UTIL_HudMessage( pPlayer, -1.0f, 0.8f, HUD_EFFECT_FADE_IN_OUT, m_clrFailedCriteriaColor, m_clrFailedCriteriaColor, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszFailedCriteriaMessage) );
				m_fNextFailedCriteriaMessage[iPlayerIndex] = gpGlobals->curtime + 1.8f;
			}
		}
	}
#endif

	return bReturnValue;
}

bool CFF_SH_BaseFFGoal::PassesCriteriaCheck( const CBaseEntity *pEnt, bool bIsActivator )
{
	// Check to see if this player needs to be on a certain team to pass.
	if( TeamCheck_IsUsed() )
	{
		if( !TeamCheck_IsTeamAllowed( pEnt->GetTeamNumber() ) )
			return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
	}
	else
	{
		if( m_iCriteria_TeamsAllowed && !( m_iCriteria_TeamsAllowed & FF_TEAM_BITS[pEnt->GetTeamNumber()] ) )
			return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
	}

	// --> START: Player specific checks.
	const CBasePlayer *pPlayer = dynamic_cast<const CBasePlayer *>(pEnt);
	if( pPlayer )
	{
		// TODO: Add this back when classes are added.
		/*
		// Check to see if this player needs to be a certain class to pass.
		if( TeamCheck_IsUsed() )
		{
			if( !TeamCheck_IsClassAllowed( pActivator->GetClass() ) )
				return PassesCriteriaCheckOutput( pEnt, bIsActivator, false);
		}
		else
		{
			if( m_iCriteria_ClassesAllowed && !( m_iCriteria_ClassesAllowed & FF_CLASS_BITS[pActivator->GetClass()] ) )
				return PassesCriteriaCheckOutput( pEnt, bIsActivator, false);
		}
		*/

		// Check to see if this player needs to be carrying specific items.
		for( int i=0; i<m_Criteria_ItemsCarriedByActivator.Count(); i++ )
		{
			if( pPlayer != m_Criteria_ItemsCarriedByActivator[i]->GetCarrier() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to see if this player needs to NOT be carrying specific items.
		for( int i=0; i<m_Criteria_ItemsNotCarriedByActivator.Count(); i++ )
		{
			if( pPlayer == m_Criteria_ItemsNotCarriedByActivator[i]->GetCarrier() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to see if this players team needs to be carrying specific items.
		CBasePlayer *pCarrier;

		for( int i=0; i<m_Criteria_ItemsCarriedByTeam.Count(); i++ )
		{
			pCarrier = m_Criteria_ItemsCarriedByTeam[i]->GetCarrier();
			if( !pCarrier || pPlayer->GetTeamNumber() != pCarrier->GetTeamNumber() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to see if this players team needs to NOT be carrying specific items.
		for( int i=0; i<m_Criteria_ItemsNotCarriedByTeam.Count(); i++ )
		{
			pCarrier = m_Criteria_ItemsNotCarriedByTeam[i]->GetCarrier();
			if( pCarrier && pPlayer->GetTeamNumber() == pCarrier->GetTeamNumber() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to make sure certain items are being carried by someone.
		for( int i=0; i<m_Criteria_ItemsCarriedByAny.Count(); i++ )
		{
			if( !m_Criteria_ItemsCarriedByAny[i]->GetCarrier() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to make sure certain items are NOT being carried by anyone.
		for( int i=0; i<m_Criteria_ItemsNotCarriedByAny.Count(); i++ )
		{
			if( m_Criteria_ItemsNotCarriedByAny[i]->GetCarrier() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to make sure certain items are being carried by one single player.
		if( m_Criteria_ItemsCarriedByOne.Count() > 0 )
		{
			pCarrier = m_Criteria_ItemsCarriedByOne[0]->GetCarrier();

			for( int i=0; i<m_Criteria_ItemsCarriedByOne.Count(); i++ )
			{
				if( !pCarrier || pCarrier != m_Criteria_ItemsCarriedByOne[i]->GetCarrier() )
					return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
			}
		}

		// Check to make sure certain items are NOT being carried by one single player.
		if( m_Criteria_ItemsNotCarriedByOne.Count() > 0 )
		{
			int iMatches = 0;
			pCarrier = m_Criteria_ItemsNotCarriedByOne[0]->GetCarrier();

			for( int i=0; i<m_Criteria_ItemsNotCarriedByOne.Count(); i++ )
			{
				if( pCarrier && pCarrier == m_Criteria_ItemsNotCarriedByOne[i]->GetCarrier() )
					iMatches++;
			}

			if( iMatches == m_Criteria_ItemsNotCarriedByOne.Count() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to make sure certain items are at their spawn point.
		for( int i=0; i<m_Criteria_ItemsAtSpawnPoint.Count(); i++ )
		{
			if( m_Criteria_ItemsAtSpawnPoint[i]->GetAbsOrigin() != m_Criteria_ItemsAtSpawnPoint[i]->GetSpawnOrigin() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}

		// Check to make sure certain items are NOT at their spawn point.
		for( int i=0; i<m_Criteria_ItemsNotAtSpawnPoint.Count(); i++ )
		{
			if( m_Criteria_ItemsNotAtSpawnPoint[i]->GetAbsOrigin() == m_Criteria_ItemsNotAtSpawnPoint[i]->GetSpawnOrigin() )
				return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
		}
	}
	// <-- END: Player specific checks.

	// Check to make sure certain goals are active.
	for( int i=0; i<m_Criteria_GoalsAreActive.Count(); i++ )
	{
		if( !m_Criteria_GoalsAreActive[i]->IsGoalActivated() )
			return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
	}

	// Check to make sure certain goals are NOT active.
	for( int i=0; i<m_Criteria_GoalsAreInactive.Count(); i++ )
	{
		if( m_Criteria_GoalsAreInactive[i]->IsGoalActivated() )
			return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
	}

	// Check to make sure certain goals are enabled.
	for( int i=0; i<m_Criteria_GoalsAreEnabled.Count(); i++ )
	{
		if( !m_Criteria_GoalsAreEnabled[i]->IsGoalEnabled() )
			return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
	}

	// Check to make sure certain goals are NOT enabled.
	for( int i=0; i<m_Criteria_GoalsAreDisabled.Count(); i++ )
	{
		if( m_Criteria_GoalsAreDisabled[i]->IsGoalEnabled() )
			return HandleCriteriaCheckResult( pEnt, bIsActivator, false);
	}

	// The activator passed all criteria checks!
	return HandleCriteriaCheckResult( pEnt, bIsActivator, true);
}

#ifdef GAME_DLL
void CFF_SH_BaseFFGoal::OnTouching( CBaseEntity *pOther )
{
	// TODO: Fix the bug that allows any entity to touch each other.
	// -->

	if( m_spawnflags & GSF_ACTIVATED_BY_PLAYER_TOUCH )
	{
		if( dynamic_cast<CBasePlayer *>(pOther) )
			TryActivateGoal( pOther );
	}
}

void CFF_SH_BaseFFGoal::TryActivateGoal( CBaseEntity *pActivator )
{
	if( !m_bGoalEnabled || m_bGoalActive )
		return;

	if( PassesCriteriaCheck( pActivator ) )
		ActivateGoalStart( pActivator );
}

void CFF_SH_BaseFFGoal::ActivateGoalStart( CBaseEntity *pActivator )
{
	m_hGoalActivator = pActivator;
	m_bGoalActive = true;

	if( m_fActiveDelay > 0.0f )
	{
		SetThink( &CFF_SH_BaseFFGoal::ActivateGoal );
		SetNextThink( gpGlobals->curtime + m_fActiveDelay );
	}
	else
	{
		ActivateGoal();
	}
}

void CFF_SH_BaseFFGoal::ActivateGoal()
{
	m_bGoalActivated = true;
	m_Output_OnActive.FireOutput( m_hGoalActivator, this ); // TODO: Do we need to check if m_hGoalActivator still exists? If not send the world ent?

	if( m_iszActivationSound != NULL_STRING )
	{
		CRecipientFilter filter;
		filter.AddRecipientsByPVS( GetAbsOrigin() );
		filter.MakeReliable();

		EmitSound_t params;
		params.m_pSoundName = STRING(m_iszActivationSound);
		params.m_SoundLevel = SNDLVL_NORM;

		if( filter.GetRecipientCount() )
			EmitSound( filter, ENTINDEX(this), params );
	}

	// Start the deactivate timer.
	if( m_fActiveTime )
	{
		SetThink( &CFF_SH_BaseFFGoal::DeactivateGoal );
		SetNextThink( gpGlobals->curtime + m_fActiveTime );
	}
	else
	{
		DeactivateGoal();
	}
}

void CFF_SH_BaseFFGoal::DeactivateGoal()
{
	m_bGoalActivated = false;
	m_Output_OnInactive.FireOutput( m_hGoalActivator, this ); // TODO: Do we need to check if m_hGoalActivator still exists? If not send the world ent?

	if( m_iszResetSound != NULL_STRING )
	{
		CRecipientFilter filter;
		filter.AddRecipientsByPVS( GetAbsOrigin() );
		filter.MakeReliable();

		EmitSound_t params;
		params.m_pSoundName = STRING(m_iszResetSound);
		params.m_SoundLevel = SNDLVL_NORM;

		if( filter.GetRecipientCount() )
			EmitSound( filter, ENTINDEX(this), params );
	}

	if( m_bGoalEnabled && m_bGoalActive && ( m_spawnflags & GSF_LOOP_ACTIVATION_UNTIL_DISABLED ) )
	{
		ActivateGoalStart( m_hGoalActivator );
	}
	else
	{
		m_bGoalActive = false;
		SetThink( NULL );
	}
}

void CFF_SH_BaseFFGoal::Precache()
{
	const char *szSound;
	if( m_iszActivationSound != NULL_STRING )
	{
		szSound = STRING(m_iszActivationSound);
		if(*szSound != '!')
			PrecacheScriptSound(szSound);
	}

	if( m_iszResetSound != NULL_STRING )
	{
		szSound = STRING(m_iszResetSound);
		if(*szSound != '!')
			PrecacheScriptSound(szSound);
	}

	BaseClass::Precache();
}

void CFF_SH_BaseFFGoal::Spawn()
{
	Precache();

	ParseCriteriaKeyValues();
	BaseClass::Activate();
}

void CFF_SH_BaseFFGoal::Activate()
{
	ParseCriteriaKeyValues();
	BaseClass::Activate();

	if( m_spawnflags & GSF_START_ACTIVE )
	{
		CBaseEntity *pWorld = Instance(0);
		ActivateGoalStart( pWorld );
	}
}

void CFF_SH_BaseFFGoal::ParseCriteriaKeyValues()
{
	// Put spawnflags that can change in their own variable.
	m_bGoalEnabled = ( m_spawnflags & GSF_START_ENABLED ) ? true : false;

	// Parse teams allowed.
	CUtlVector<char *> string_vector;
	Q_SplitString( STRING(m_iszCriteria_TeamsAllowed), ",", string_vector );

	int iTemp;
	for( int i=0; i<string_vector.Count(); i++ )
	{
		iTemp = Q_atoi(string_vector[i]);
		if( iTemp == -1 )
		{
			m_iCriteria_TeamsAllowed = -1;
			break;
		}

		iTemp++; // Increase the team by 1 since FF_TEAM_ONE starts at value 2.
		if( iTemp < FF_TEAM_ONE || iTemp > FF_TEAM_LAST )
			continue;

		m_iCriteria_TeamsAllowed |= FF_TEAM_BITS[iTemp];
	}

	// Parse classes allowed.
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ClassesAllowed), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		iTemp = Q_atoi(string_vector[i]);
		if( iTemp == -1 )
		{
			m_iCriteria_ClassesAllowed = -1;
			break;
		}

		if( iTemp <= FF_CLASS_UNASSIGNED || iTemp >= FF_CLASS_COUNT )
			continue;

		m_iCriteria_ClassesAllowed |= FF_CLASS_BITS[iTemp];
	}

	// Parse item/goal based criterias.
	CBaseEntity *pEnt;
	CFF_SH_BaseFFGoal *pGoal;
	CFF_SH_ItemFFGoal *pItem;

	// ItemsCarriedByActivator
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsCarriedByActivator), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsCarriedByActivator.AddToTail( pItem );
		}
	}

	// ItemsNotCarriedByActivator
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsNotCarriedByActivator), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsNotCarriedByActivator.AddToTail( pItem );
		}
	}

	// ItemsCarriedByTeam
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsCarriedByTeam), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsCarriedByTeam.AddToTail( pItem );
		}
	}

	// ItemsNotCarriedByTeam
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsNotCarriedByTeam), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsNotCarriedByTeam.AddToTail( pItem );
		}
	}

	// ItemsCarriedByAny
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsCarriedByAny), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsCarriedByAny.AddToTail( pItem );
		}
	}

	// ItemsNotCarriedByAny
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsNotCarriedByAny), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsNotCarriedByAny.AddToTail( pItem );
		}
	}

	// ItemsCarriedByOne
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsCarriedByOne), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsCarriedByOne.AddToTail( pItem );
		}
	}

	// ItemsNotCarriedByOne
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsNotCarriedByOne), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsNotCarriedByOne.AddToTail( pItem );
		}
	}

	// ItemsAtSpawnPoint
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsAtSpawnPoint), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsAtSpawnPoint.AddToTail( pItem );
		}
	}

	// ItemsNotAtSpawnPoint
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_ItemsNotAtSpawnPoint), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pItem = dynamic_cast<CFF_SH_ItemFFGoal *>(pEnt);
			if( pItem )
				m_Criteria_ItemsNotAtSpawnPoint.AddToTail( pItem );
		}
	}

	// GoalsAreActive
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_GoalsAreActive), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pGoal = dynamic_cast<CFF_SH_BaseFFGoal *>(pEnt);
			if( pGoal )
				m_Criteria_GoalsAreActive.AddToTail( pGoal );
		}
	}

	// GoalsAreInactive
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_GoalsAreInactive), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pGoal = dynamic_cast<CFF_SH_BaseFFGoal *>(pEnt);
			if( pGoal )
				m_Criteria_GoalsAreInactive.AddToTail( pGoal );
		}
	}

	// GoalsAreEnabled
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_GoalsAreEnabled), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pGoal = dynamic_cast<CFF_SH_BaseFFGoal *>(pEnt);
			if( pGoal )
				m_Criteria_GoalsAreEnabled.AddToTail( pGoal );
		}
	}

	// GoalsAreDisabled
	string_vector.Purge();
	Q_SplitString( STRING(m_iszCriteria_GoalsAreDisabled), ",", string_vector );

	for( int i=0; i<string_vector.Count(); i++ )
	{
		pEnt = NULL;
		while( (pEnt = gEntList.FindEntityByName( pEnt, string_vector[i] )) != NULL )
		{
			pGoal = dynamic_cast<CFF_SH_BaseFFGoal *>(pEnt);
			if( pGoal )
				m_Criteria_GoalsAreDisabled.AddToTail( pGoal );
		}
	}
}

void CFF_SH_BaseFFGoal::Input_Enable( inputdata_t &inputdata )
{
	m_bGoalEnabled = true;
	m_Output_OnEnabled.FireOutput( inputdata.pActivator, this );
}

void CFF_SH_BaseFFGoal::Input_Disable( inputdata_t &inputdata )
{
	m_bGoalEnabled = false;
	m_Output_OnDisabled.FireOutput( inputdata.pActivator, this );
}

void CFF_SH_BaseFFGoal::Input_Activate( inputdata_t &inputdata )
{
	if( !m_bGoalEnabled || m_bGoalActive )
		return;

	CBaseEntity *pWorld = Instance(0);
	ActivateGoalStart( pWorld );
}

void CFF_SH_BaseFFGoal::Input_Inactivate( inputdata_t &inputdata )
{
	if( !m_bGoalEnabled || !m_bGoalActive )
		return;

	m_bGoalActive = false; // WARNING: We must set m_bGoalActive to false before calling DeactivateGoal() because it will start looping again if looping is set.
	DeactivateGoal();
}
#endif