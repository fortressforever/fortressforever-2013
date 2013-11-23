#include "cbase.h"
#include "ff_sv_env_ff_message.h"
#include "ff_sv_util.h"


LINK_ENTITY_TO_CLASS( env_ff_message, CFF_SV_EnvFFMessage );

BEGIN_DATADESC( CFF_SV_EnvFFMessage )
	// Keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToAll, FIELD_STRING, "msg_to_all" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToActivator, FIELD_STRING, "msg_to_activator" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToNonActivator, FIELD_STRING, "msg_to_non_activator" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToActivatorsTeams, FIELD_STRING, "msg_to_activators_teams" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToNonActivatorsTeams, FIELD_STRING, "msg_to_non_activators_teams" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToOwnerTeams, FIELD_STRING, "msg_to_owner_teams" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToNonOwnerTeams, FIELD_STRING, "msg_to_non_owner_teams" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgAll", Input_ShowMsgAll ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgActivator", Input_ShowMsgActivator ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgNonActivator", Input_ShowMsgNonActivator ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgActivatorTeams", Input_ShowMsgActivatorTeams ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgNonActivatorTeams", Input_ShowMsgNonActivatorTeams ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgOwnerTeams", Input_ShowMsgOwnerTeams ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgNonOwnerTeams", Input_ShowMsgNonOwnerTeams ),
END_DATADESC()


void CFF_SV_EnvFFMessage::Input_ShowMessages( inputdata_t &inputdata )
{
	// TODO:
	DevMsg("Implement Input_ShowMessages\n");
	return;

	// TODO: We need to make an array of all players connected, and when we send a message to one of them we need to remove them from the array so they can't have a lower priority msg sent to them.

	// Priority: activator, (activator teams, non activator team), (owner teams, non owner teams), non activator, all
	CRecipientFilter filter;
	CBasePlayer *pActivator = dynamic_cast<CBasePlayer *>(inputdata.pActivator);

	if( pActivator )
	{
		// Message to activator.
		if( m_iszMessageToActivator != NULL_STRING )
		{
			filter.AddRecipient(pActivator);
			FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToActivator) );
			return;
		}

		// Message to activator teams.
		if( m_iszMessageToActivatorsTeams != NULL_STRING )
		{
			filter.AddRecipientsByTeam( pActivator->GetTeam() );

			// TODO: Add allied teams.
			// -->

			FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToActivatorsTeams) );
			return;
		}

		// Message to non-activator teams.
		if( m_iszMessageToNonActivatorsTeams != NULL_STRING )
		{
			filter.AddAllPlayers();
			filter.RemoveRecipientsByTeam( pActivator->GetTeam() );

			// TODO: Remove allied teams.
			// -->

			FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToNonActivatorsTeams) );
			return;
		}
	}

	// TODO: Implement owner teams and non-owner teams when goals are finished.
	// -->

	if( m_iszMessageToNonActivator != NULL_STRING )
	{
		//
	}
}

void CFF_SV_EnvFFMessage::Input_ShowMsgAll( inputdata_t &inputdata )
{
	if( !m_iszMessageToAll )
		return;

	FF_UTIL_HudMessageAll( -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToAll) );
}

void CFF_SV_EnvFFMessage::Input_ShowMsgActivator( inputdata_t &inputdata )
{
	if( !m_iszMessageToActivator )
		return;

	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( !pPlayer )
		return;

	CRecipientFilter filter;
	filter.AddRecipient( pPlayer );
	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToActivator) );
}

void CFF_SV_EnvFFMessage::Input_ShowMsgNonActivator( inputdata_t &inputdata )
{
	if( !m_iszMessageToNonActivator )
		return;

	CRecipientFilter filter;
	filter.AddAllPlayers();

	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( pPlayer )
		filter.RemoveRecipient(pPlayer);

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToNonActivator) );
}

void CFF_SV_EnvFFMessage::Input_ShowMsgActivatorTeams( inputdata_t &inputdata )
{
	if( !m_iszMessageToActivatorsTeams )
		return;

	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( !pPlayer )
		return;

	CRecipientFilter filter;
	filter.AddRecipientsByTeam( pPlayer->GetTeam() );

	// TODO: Add allied teams.
	// -->

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToActivatorsTeams) );
}

void CFF_SV_EnvFFMessage::Input_ShowMsgNonActivatorTeams( inputdata_t &inputdata )
{
	if( !m_iszMessageToNonActivatorsTeams )
		return;

	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( !pPlayer )
		return;

	CRecipientFilter filter;
	filter.AddAllPlayers();
	filter.RemoveRecipientsByTeam( pPlayer->GetTeam() );

	// TODO: Remove allied teams.
	// -->

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToNonActivatorsTeams) );
}

void CFF_SV_EnvFFMessage::Input_ShowMsgOwnerTeams( inputdata_t &inputdata )
{
	// TODO: Need goal entity implemented first.
	DevMsg("Implement Input_ShowMsgOwnerTeams\n");
}

void CFF_SV_EnvFFMessage::Input_ShowMsgNonOwnerTeams( inputdata_t &inputdata )
{
	// TODO: Need goal entity implemented first.
	DevMsg("Implement Input_ShowMsgNonOwnerTeams\n");
}
