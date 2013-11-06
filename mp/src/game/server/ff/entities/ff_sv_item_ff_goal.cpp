#include "cbase.h"
#include "ff_sv_util.h"
#include "ff_sv_teamcheck_target.h"


class CFF_SV_ItemFFGoal : public CFF_SV_TeamcheckTarget
{
public:
	DECLARE_CLASS( CFF_SV_ItemFFGoal, CFF_SV_TeamcheckTarget );
	DECLARE_DATADESC();

	CFF_SV_ItemFFGoal()
	{
		SetThink( NULL );

		m_bEnabled = true;
		m_bActive = false;

		m_fActiveTime = 1.5f;
		m_fActiveDelay = 0.0f;
	}

	void Precache( void );
	void Spawn( void );

	void OnTouch( CBaseEntity *pOther );

	void SetActive( CBasePlayer *pPlayer );
	void SetInactive( void );

private:
	COutputEvent m_OnStartTouch;
	COutputEvent m_OnEndTouch;

	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputSetActive( inputdata_t &inputdata );
	void InputSetInactive( inputdata_t &inputdata );

	void ThinkDoActive( void );
	void ThinkSetInactive( void );

	bool m_bEnabled;		// If disabled then the goal will ignore all Inputs (except for Enable).
	bool m_bActive;			// Is the goal active?

	float m_fActiveTime;	// How long to stay in the active state.
	float m_fActiveDelay;	// How long to wait before going in the active state.

	EHANDLE m_hGoalActivator;	// The player that activated this goal (this is not always the owner).
	EHANDLE m_hGoalOwner;		// The player that owns this goal (this is not always the activator).

	string_t m_iszActivatedMsgToAll;		// The message sent to everyone when the goal is activated.
	string_t m_iszActivatedMsgToOwner;		// The message sent to the owner when the goal is activated.

	string_t m_iszSoundName;
};


LINK_ENTITY_TO_CLASS( item_ff_goal, CFF_SV_ItemFFGoal );

BEGIN_DATADESC( CFF_SV_ItemFFGoal )
	// Goal keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_bEnabled, FIELD_BOOLEAN, "enabled" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_bActive, FIELD_BOOLEAN, "active" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_fActiveTime, FIELD_FLOAT, "active_time" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_fActiveDelay, FIELD_FLOAT, "active_delay" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszActivatedMsgToAll, FIELD_STRING, "msg_activated_to_all" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_iszActivatedMsgToOwner, FIELD_STRING, "msg_activated_to_owner" ),

	DEFINE_KEYFIELD_NOT_SAVED( m_iszSoundName, FIELD_SOUNDNAME, "sound" ),

	// Goal touch functions.
	DEFINE_FUNCTION( OnTouch ),

	// Goal think functions.
	DEFINE_THINKFUNC( ThinkDoActive ),
	DEFINE_THINKFUNC( ThinkSetInactive ),

	// Goal inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SetActive", InputSetActive ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SetInactive", InputSetInactive ),
END_DATADESC()


void CFF_SV_ItemFFGoal::Precache( void )
{
	if( GetModelName().ToCStr()[0] )
		PrecacheModel( GetModelName().ToCStr() );

	const char *szSound = m_iszSoundName.ToCStr();
	if( m_iszSoundName != NULL_STRING )
	{
		if (*szSound != '!')
			PrecacheScriptSound(szSound);
	}

	BaseClass::Precache();
}

void CFF_SV_ItemFFGoal::Spawn( void )
{
	Precache();

	if( GetModelName().ToCStr()[0] )
		SetModel( GetModelName().ToCStr() );

	SetMoveType( MOVETYPE_NONE );
	//SetSolid( SOLID_VPHYSICS ); // WARNING: If the entity doesn't have a model then we MUST use SOLID_BBOX or the game will crash.
	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_SOLID | FSOLID_TRIGGER );

	SetTouch( &CFF_SV_ItemFFGoal::OnTouch );
}

void CFF_SV_ItemFFGoal::OnTouch( CBaseEntity *pOther )
{
	// TODO: Should we just hook this function in StartTouch() instead?

	if(!m_bEnabled || m_bActive)
		return;

	// Make sure pOther is a player.
	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(pOther);
	if(!pPlayer)
		return;

	SetActive(pPlayer);
}

void CFF_SV_ItemFFGoal::InputEnable( inputdata_t &inputdata )
{
	m_bEnabled = true;
}

void CFF_SV_ItemFFGoal::InputDisable( inputdata_t &inputdata )
{
	m_bEnabled = false;
}

void CFF_SV_ItemFFGoal::InputSetActive( inputdata_t &inputdata )
{
	if(!m_bEnabled || m_bActive)
		return;

	SetActive( dynamic_cast<CBasePlayer *>(inputdata.pActivator) );
}

void CFF_SV_ItemFFGoal::InputSetInactive( inputdata_t &inputdata )
{
	if(!m_bEnabled || !m_bActive)
		return;

	SetInactive();
}

void CFF_SV_ItemFFGoal::SetActive( CBasePlayer *pPlayer )
{
	if( !IsTeamAllowed( pPlayer->GetTeamNumber() ) )
		return;

	m_hGoalActivator = pPlayer;
	m_bActive = true;

	if(m_fActiveDelay)
	{
		SetThink( &CFF_SV_ItemFFGoal::ThinkDoActive );
		SetNextThink( gpGlobals->curtime + m_fActiveDelay );
	}
	else
	{
		ThinkDoActive();
	}
}

void CFF_SV_ItemFFGoal::SetInactive()
{
	RemoveEffects( EF_NODRAW );

	m_bActive = false;

	SetThink( NULL );
}

void CFF_SV_ItemFFGoal::ThinkDoActive()
{
	AddEffects( EF_NODRAW );

	CBasePlayer *pActivator = dynamic_cast<CBasePlayer *>(m_hGoalActivator.Get());
	if( pActivator )
	{
		color32 color = {200, 200, 200, 255};
		FF_UTIL_HudMessageFormat( pActivator, -1.0f, 0.7f, HUD_EFFECT_FADE_IN_OUT, color, color, 0.2f, 0.3f, 1.3f, 0.0f, 1, STRING(m_iszActivatedMsgToOwner) );
	}

	if( m_iszSoundName.ToCStr()[0] )
	{
		CRecipientFilter filter;
		filter.AddRecipientsByPVS( GetAbsOrigin() );
		filter.MakeReliable();

		EmitSound_t params;
		params.m_pSoundName = m_iszSoundName.ToCStr();

		EmitSound( filter, ENTINDEX(this), params );
	}

	// Start setting inactive.
	if(m_fActiveTime)
	{
		SetThink( &CFF_SV_ItemFFGoal::ThinkSetInactive );
		SetNextThink( gpGlobals->curtime + m_fActiveTime );
	}
	else
	{
		ThinkSetInactive();
	}
}

void CFF_SV_ItemFFGoal::ThinkSetInactive()
{
	SetInactive();
}
