#include "cbase.h"


class CFF_CL_InfoFFWeaponSpawner : public C_BaseAnimating
{
public:
	DECLARE_CLASS( CFF_CL_InfoFFWeaponSpawner, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	CFF_CL_InfoFFWeaponSpawner()
	{
		m_angles.y = RandomFloat(0.0f, 359.0f);
		m_bDirection = RandomInt(0, 1);
	}

	void Spawn();
	void ClientThink();

private:
	QAngle	m_angles;
	bool	m_bDirection;
};


LINK_ENTITY_TO_CLASS( info_ff_weapon_spawner, CFF_CL_InfoFFWeaponSpawner );

IMPLEMENT_CLIENTCLASS_DT(CFF_CL_InfoFFWeaponSpawner, DT_FF_InfoFFWeaponSpawner, CFF_SV_InfoFFWeaponSpawner)
END_RECV_TABLE()


void CFF_CL_InfoFFWeaponSpawner::Spawn()
{
	SetNextClientThink(CLIENT_THINK_ALWAYS);
	ClientThink();
}

void CFF_CL_InfoFFWeaponSpawner::ClientThink()
{
	m_angles.x = -30.0f;

	if(m_bDirection)
	{
		m_angles.y += 360.0f / 3.5f * gpGlobals->frametime;
		if(m_angles.y >= 360)
			m_angles.y -= 360;
	}
	else
	{
		m_angles.y -= 360.0f / 3.5f * gpGlobals->frametime;
		if(m_angles.y < 0)
			m_angles.y += 360;
	}

	SetLocalAngles(m_angles);
}