#ifndef FF_SH_VARS_MOVEMENT_H
#define FF_SH_VARS_MOVEMENT_H
#pragma once

#include "convar.h"

extern bool g_bMovementOptimizations;

#define HEADCRUSH_DAMAGE 108.0f				// Straight headcrush damage; not used if usefalldamage is on
#define HEADCRUSH_USEFALLDAMAGE 4.0f		// 0 = off, > 0 means take FALLDAMAGE * val damage

#define SV_TRIMPMULTIPLIER 1.4f
#define SV_TRIMPDOWNMULTIPLIER 1.2f
#define SV_TRIMPMAX 5000.0f
#define SV_TRIMPTRIGGERSPEED 550.0f
#define SV_TRIMPTRIGGERSPEEDDOWN 50.0f

#define BHOP_CAP_SOFT 1.4f
#define BHOP_PCFACTOR 0.65f
#define BHOP_CAP_HARD 1.8f

#ifdef CLIENT_DLL
extern ConVar cl_jumpqueue;
#endif

#endif // FF_SH_VARS_MOVEMENT_H
