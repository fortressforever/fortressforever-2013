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
		m_nCurAlpha = 255;
	}

	void			Spawn();
	void			ClientThink();
	unsigned char	GetClientSideFade();
	void			OnDataChanged(DataUpdateType_t updateType);
	void			OnPreDataChanged(DataUpdateType_t updateType);

private:
	QAngle	m_angles;
	bool	m_bDirection;
	int		m_nCurAlpha; // NOTE: This isn't an unsigned char because when adding it can go over 255 and overflow which messes our calculations up.

	int		m_bOldEffects;
};


LINK_ENTITY_TO_CLASS( info_ff_weapon_spawner, CFF_CL_InfoFFWeaponSpawner );

IMPLEMENT_CLIENTCLASS_DT(CFF_CL_InfoFFWeaponSpawner, DT_FF_InfoFFWeaponSpawner, CFF_SV_InfoFFWeaponSpawner)
END_RECV_TABLE()


void CFF_CL_InfoFFWeaponSpawner::OnPreDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnPreDataChanged(updateType);
	m_bOldEffects = GetEffects();
}

void CFF_CL_InfoFFWeaponSpawner::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);
	if(updateType != DATA_UPDATE_DATATABLE_CHANGED)
		return;

	if(!(GetEffects() & EF_NODRAW) && GetEffects() != m_bOldEffects)
		m_nCurAlpha = 0;
}

unsigned char CFF_CL_InfoFFWeaponSpawner::GetClientSideFade()
{
	m_nCurAlpha += RoundFloatToInt((255.0f / 0.27f) * gpGlobals->frametime);
	if(m_nCurAlpha > 255)
		m_nCurAlpha = 255;

	unsigned char alpha = BaseClass::GetClientSideFade();
	if(alpha < m_nCurAlpha)
		return alpha;

	return m_nCurAlpha;
}

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