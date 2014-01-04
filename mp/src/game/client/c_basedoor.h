//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#if !defined( C_BASEDOOR_H )
#define C_BASEDOOR_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseentity.h"
#include "ff_sh_base_ff_goal.h" // FF: --> hlstriker: Added

#if defined( CLIENT_DLL )
#define CBaseDoor C_BaseDoor
#endif

class C_BaseDoor : public CFF_SH_BaseFFGoal // FF: --> hlstriker: Changed to derive from CFF_SH_BaseFFGoal. It was C_BaseEntity.
//class C_BaseDoor : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_BaseDoor, CFF_SH_BaseFFGoal ); // FF: --> hlstriker: Changed to derive from CFF_SH_BaseFFGoal. It was C_BaseEntity.
	//DECLARE_CLASS( C_BaseDoor, C_BaseEntity );
	DECLARE_CLIENTCLASS();

	C_BaseDoor( void );
	~C_BaseDoor( void );

public:
	float		m_flWaveHeight;
};

#endif // C_BASEDOOR_H