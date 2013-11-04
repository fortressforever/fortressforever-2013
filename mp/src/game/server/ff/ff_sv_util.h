#pragma once

#ifndef FF_SV_UTIL_H
#define FF_SV_UTIL_H

// HUD Messages
typedef enum 
{
	HUD_EFFECT_FADE_IN_OUT = 0,
	HUD_EFFECT_FLICKER,
	HUD_EFFECT_WRITE_OUT
} HUD_MSG_EFFECT;

void	FF_UTIL_HudMessage( CBasePlayer *pToPlayer, float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszMessage );
void	FF_UTIL_HudMessageFormat( CBasePlayer *pToPlayer, float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszFormat, ... );
void	FF_UTIL_HudMessageAll( float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszMessage );
void	FF_UTIL_HudMessageAllFormat( float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszFormat, ... );


#endif //FF_SV_UTIL_H
