#include "cbase.h"
#include "ff_sv_util.h"


class CFF_SV_PointFFMessage : public CBaseEntity
{
public:
	DECLARE_CLASS( CFF_SV_PointFFMessage, CBaseEntity );
	DECLARE_DATADESC();

	CFF_SV_PointFFMessage()
	{
		m_clrMesssage.r = 200;
		m_clrMesssage.g = 200;
		m_clrMesssage.b = 200;
		m_clrMesssage.a = 255;
	}

private:
	color32 m_clrMesssage;

	string_t m_iszMessageToAll;
	string_t m_iszMessageToActivator;
	string_t m_iszMessageToNonActivator;
	string_t m_iszMessageToActivatorsTeams;
	string_t m_iszMessageToNonActivatorsTeam;

	void Input_ShowMsgAll( inputdata_t &inputdata );
	void Input_ShowMsgActivator( inputdata_t &inputdata );
	void Input_ShowMsgNonActivator( inputdata_t &inputdata );
	void Input_ShowMsgActivatorTeam( inputdata_t &inputdata );
	void Input_ShowMsgNonActivatorTeam( inputdata_t &inputdata );

	void ShowDefaultMessage( CRecipientFilter &filter );
};


LINK_ENTITY_TO_CLASS( point_ff_message, CFF_SV_PointFFMessage );

BEGIN_DATADESC( CFF_SV_PointFFMessage )
	// Goal keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToAll, FIELD_STRING, "msg_to_all" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToActivator, FIELD_STRING, "msg_to_activator" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToNonActivator, FIELD_STRING, "msg_to_non_activator" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToActivatorsTeams, FIELD_STRING, "msg_to_activators_team" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszMessageToNonActivatorsTeam, FIELD_STRING, "msg_to_non_activators_team" ),

	// Goal inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgAll", Input_ShowMsgAll ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgActivator", Input_ShowMsgActivator ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgNonActivator", Input_ShowMsgNonActivator ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgActivatorTeam", Input_ShowMsgActivatorTeam ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ShowMsgNonActivatorTeam", Input_ShowMsgNonActivatorTeam ),
END_DATADESC()


void CFF_SV_PointFFMessage::ShowDefaultMessage( CRecipientFilter &filter )
{
	if( !m_iszMessageToAll )
		return;

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToAll) );
}

void CFF_SV_PointFFMessage::Input_ShowMsgAll( inputdata_t &inputdata )
{
	if( !m_iszMessageToAll )
		return;

	FF_UTIL_HudMessageAll( -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToAll) );
}

void CFF_SV_PointFFMessage::Input_ShowMsgActivator( inputdata_t &inputdata )
{
	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( !pPlayer )
		return;

	CRecipientFilter filter;
	filter.AddRecipient( pPlayer );

	if( !m_iszMessageToActivator )
	{
		ShowDefaultMessage( filter );
		return;
	}

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToActivator) );
}

void CFF_SV_PointFFMessage::Input_ShowMsgNonActivator( inputdata_t &inputdata )
{
	CRecipientFilter filter;
	filter.AddAllPlayers();

	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( pPlayer )
		filter.RemoveRecipient(pPlayer);

	if( !m_iszMessageToNonActivator )
	{
		ShowDefaultMessage( filter );
		return;
	}

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToNonActivator) );
}

void CFF_SV_PointFFMessage::Input_ShowMsgActivatorTeam( inputdata_t &inputdata )
{
	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( !pPlayer )
		return;

	CRecipientFilter filter;
	filter.AddRecipientsByTeam( pPlayer->GetTeam() );

	if( !m_iszMessageToActivatorsTeams )
	{
		ShowDefaultMessage( filter );
		return;
	}

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToActivatorsTeams) );
}

void CFF_SV_PointFFMessage::Input_ShowMsgNonActivatorTeam( inputdata_t &inputdata )
{
	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(inputdata.pActivator);
	if( !pPlayer )
		return;

	CRecipientFilter filter;
	filter.AddAllPlayers();
	filter.RemoveRecipientsByTeam( pPlayer->GetTeam() );

	if( !m_iszMessageToNonActivatorsTeam )
	{
		ShowDefaultMessage( filter );
		return;
	}

	FF_UTIL_HudMessage( filter, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, m_clrMesssage, m_clrMesssage, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszMessageToNonActivatorsTeam) );
}
