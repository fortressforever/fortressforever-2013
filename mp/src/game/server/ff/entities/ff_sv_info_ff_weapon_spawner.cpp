#include "cbase.h"
#include "ff_sv_teamcheck_target.h"
#include "ff_sh_shareddefs.h"
#include "ammodef.h"


class CFF_SV_InfoFFWeaponSpawner : public CFF_SV_TeamcheckTarget
{
public:
	DECLARE_CLASS( CFF_SV_InfoFFWeaponSpawner, CFF_SV_TeamcheckTarget );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CFF_SV_InfoFFWeaponSpawner()
	{
		m_iCurIndex = 0;
		SetThink(NULL);
	}

	void Spawn( void );
	void ForceNextSpawn( void );
	void SetWait( void );
	void ThinkNextSpawn( void );

	void OnTouch( CBaseEntity *pOther );

private:
	int		m_iWeaponsAllowed;
	float	m_fRespawnTime;
	float	m_fGiveAmmoScale;	// 0.0 - 1.0 where 1.0 is 100% of that weapons max ammo.
	bool	m_bSpawnMode;		// False = Shuffle. True = Random.

	int		m_iCurIndex;		// The current index in m_iWeaponArray. This is only used if m_bSpawnMode is set to 'shuffle' (false).
	int		m_iNumInArray;
	int		m_iWeaponArray[FF_WEAPON_COUNT];

	void	DeletePreviousWeapon( void );
	CHandle<CBaseCombatWeapon>	m_hWeapon;
};


LINK_ENTITY_TO_CLASS( info_ff_weapon_spawner, CFF_SV_InfoFFWeaponSpawner );

BEGIN_DATADESC( CFF_SV_InfoFFWeaponSpawner )
	// Keyfields.
	DEFINE_KEYFIELD_NOT_SAVED( m_iWeaponsAllowed, FIELD_INTEGER, "weapons_allowed" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_fRespawnTime, FIELD_FLOAT, "respawn_time" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_fGiveAmmoScale, FIELD_FLOAT, "give_ammo_scale" ),
	DEFINE_KEYFIELD_NOT_SAVED( m_bSpawnMode, FIELD_BOOLEAN, "spawn_mode" ),

	// Touch functions.
	DEFINE_FUNCTION( OnTouch ),

	// Think functions.
	DEFINE_THINKFUNC( ThinkNextSpawn ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFF_SV_InfoFFWeaponSpawner, DT_FF_InfoFFWeaponSpawner )
	SendPropExclude("DT_BaseEntity", "m_angRotation"),
	SendPropExclude("DT_BaseEntity", "m_angAbsRotation"),
END_SEND_TABLE()


void CFF_SV_InfoFFWeaponSpawner::Spawn()
{
	m_iNumInArray = 0;
	for(int iWeaponNum=0; iWeaponNum<sizeof(FF_WEAPON_BITS) && iWeaponNum<FF_WEAPON_COUNT; iWeaponNum++)
	{
		if(m_iWeaponsAllowed & FF_WEAPON_BITS[iWeaponNum])
			m_iWeaponArray[m_iNumInArray++] = iWeaponNum;

		if(m_iNumInArray >= sizeof(m_iWeaponArray))
			break;
	}

	if(!m_bSpawnMode && m_iNumInArray > 1)
	{
		// Shuffle the weapons in the array.
		int j, t;
		for(int i=0; i<m_iNumInArray - 1; i++)
		{
			j = i + rand() / (VALVE_RAND_MAX / (m_iNumInArray - i) + 1);
			t = m_iWeaponArray[j];
			m_iWeaponArray[j] = m_iWeaponArray[i];
			m_iWeaponArray[i] = t;
		}
	}

	// No touching or displaying yet.
	SetWait();

	// Set the spawners BBOX.
	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_SOLID | FSOLID_TRIGGER);
	UTIL_SetSize(this, -Vector(32,32,32), Vector(32,32,32));

	// Go ahead and 'spawn' the initial weapon for this spawner.
	ForceNextSpawn();
}

void CFF_SV_InfoFFWeaponSpawner::OnTouch( CBaseEntity *pOther )
{
	if(!m_hWeapon)
		return;

	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(pOther);
	if(!pPlayer)
		return;

	SetWait();

	int ammoToGive = ( GetAmmoDef()->MaxCarry(m_hWeapon->GetPrimaryAmmoType()) + m_hWeapon->GetMaxClip1() ) * m_fGiveAmmoScale;
	CBaseCombatWeapon *pOwnedWeapon = pPlayer->Weapon_OwnsThisType(m_hWeapon->GetName());

	// See if they don't own this weapon yet.
	if(!pOwnedWeapon)
	{
		// Fill the new one with as much ammo as possible.
		ammoToGive -= m_hWeapon->GetMaxClip1();

		if(ammoToGive < 0)
			m_hWeapon->m_iClip1 = m_hWeapon->GetMaxClip1() + ammoToGive;
		else
			m_hWeapon->m_iClip1 = m_hWeapon->GetMaxClip1();
	}
	/*
	// TODO: Not sure if we should keep this part or not.
	else if(pOwnedWeapon->m_iClip1 == 0 && pPlayer->GetAmmoCount(m_hWeapon->GetPrimaryAmmoType()) == 0)
	{
		// They are completely out of both clip and reserve ammo. Go ahead and fill their current weapons clip to full.
		ammoToGive -= pOwnedWeapon->GetMaxClip1();

		if(ammoToGive < 0)
			pOwnedWeapon->m_iClip1 = pOwnedWeapon->GetMaxClip1() + ammoToGive;
		else
			pOwnedWeapon->m_iClip1 = pOwnedWeapon->GetMaxClip1();
	}
	*/

	// Put the rest of the ammo in their reserve.
	if(ammoToGive > 0)
		pPlayer->GiveAmmo(ammoToGive, m_hWeapon->GetPrimaryAmmoType(), true);

	m_hWeapon->SetLocalOrigin( pPlayer->GetLocalOrigin() );
	m_hWeapon->Touch(pPlayer);
	m_hWeapon = NULL;
}

void CFF_SV_InfoFFWeaponSpawner::SetWait()
{
	SetTouch(NULL);
	AddEffects(EF_NODRAW);

	SetNextThink( gpGlobals->curtime + m_fRespawnTime );
	SetThink( &CFF_SV_InfoFFWeaponSpawner::ThinkNextSpawn );
}

void CFF_SV_InfoFFWeaponSpawner::ThinkNextSpawn()
{
	ForceNextSpawn();
}

void CFF_SV_InfoFFWeaponSpawner::ForceNextSpawn()
{
	SetThink(NULL);

	if(!m_iNumInArray)
		return;

	if(m_bSpawnMode)
	{
		// Spawn mode is random.
		m_iCurIndex = RandomInt(0, m_iNumInArray-1);
	}
	else
	{
		// Spawn mode is shuffle.
		m_iCurIndex++;
		if(m_iCurIndex >= m_iNumInArray)
			m_iCurIndex = 0;
	}

	if(m_iCurIndex >= m_iNumInArray)
		return;

	DeletePreviousWeapon();

	m_hWeapon = dynamic_cast<CBaseCombatWeapon *>(CreateEntityByName( STRING( FF_WEAPON_NAME[m_iWeaponArray[m_iCurIndex]] ) ));
	if(!m_hWeapon)
		return;

	DispatchSpawn(m_hWeapon);
	m_hWeapon->AddEffects(EF_NODRAW);

	m_hWeapon->VPhysicsDestroyObject();
	m_hWeapon->UseClientSideAnimation();

	m_hWeapon->SetMoveType(MOVETYPE_NONE);
	m_hWeapon->SetSolid(SOLID_NONE);
	m_hWeapon->SetSolidFlags(FSOLID_NOT_SOLID);
	m_hWeapon->AddSpawnFlags(SF_NORESPAWN);

	m_hWeapon->m_iClip1 = 0;
	m_hWeapon->m_iClip2 = 0;
	m_hWeapon->SetPrimaryAmmoCount(0);
	m_hWeapon->SetSecondaryAmmoCount(0);

	string_t modelName = m_hWeapon->GetModelName();
	SetModel(STRING(modelName));

	RemoveEffects(EF_NODRAW);
	SetTouch( &CFF_SV_InfoFFWeaponSpawner::OnTouch );
}

void CFF_SV_InfoFFWeaponSpawner::DeletePreviousWeapon()
{
	if(!m_hWeapon)
		return;

	m_hWeapon->AddFlag(FL_KILLME);
}