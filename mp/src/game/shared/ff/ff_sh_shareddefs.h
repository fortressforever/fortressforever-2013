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
	FF_CLASS_UNASSIGNED = 0,
	FF_CLASS_SCOUT,
	FF_CLASS_SNIPER,
	FF_CLASS_SOLDIER,
	FF_CLASS_DEMOMAN,
	FF_CLASS_MEDIC,
	FF_CLASS_HWGUY,
	FF_CLASS_PYRO,
	FF_CLASS_SPY,
	FF_CLASS_ENGINEER,
	FF_CLASS_CIVILIAN,
	FF_CLASS_COUNT
} FF_CLASS;

#endif // FF_SH_SHAREDDEFS_H