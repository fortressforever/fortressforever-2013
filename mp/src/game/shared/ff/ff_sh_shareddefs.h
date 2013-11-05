#ifndef FF_SH_SHAREDDEFS_H
#define FF_SH_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

// FF Team stuff
// note, when ported i switched to an enum so we have our custom team stuff a little more obvious
typedef enum 
{
	TEAM_BLUE = 2,
	TEAM_RED,
	TEAM_YELLOW,
	TEAM_GREEN,
	TEAM_CUSTOM1,
	TEAM_CUSTOM2,
	TEAM_CUSTOM3,
	TEAM_CUSTOM4,
	TEAM_CUSTOM5,
	TEAM_CUSTOM6,
	TEAM_CUSTOM7,
	TEAM_CUSTOM8,
	TEAM_COUNT
} FF_TEAM;


typedef enum 
{
	CLASS_UNASSIGNED = 0,
	CLASS_SCOUT,
	CLASS_SNIPER,
	CLASS_SOLDIER,
	CLASS_DEMOMAN,
	CLASS_MEDIC,
	CLASS_HWGUY,
	CLASS_PYRO,
	CLASS_SPY,
	CLASS_ENGINEER,
	CLASS_CIVILIAN,
	CLASS_COUNT
} FF_CLASS;

#endif // FF_SH_SHAREDDEFS_H