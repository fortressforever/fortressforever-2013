#include "cbase.h"
#include "util.h"
#include "ff_sv_util.h"


void FF_UTIL_HudMessageAll(float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszMessage)
{
	FF_UTIL_HudMessage(NULL, x, y, effect, color1, color2, fadeInTime, fadeOutTime, holdTime, fxTime, channel, pszMessage);
}

void FF_UTIL_HudMessageAllFormat(float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszFormat, ...)
{
	char szMessage[512];
	va_list argptr;
	va_start(argptr, pszFormat);
	Q_vsnprintf(szMessage, sizeof(szMessage), pszFormat, argptr);
	va_end(argptr);

	FF_UTIL_HudMessage(NULL, x, y, effect, color1, color2, fadeInTime, fadeOutTime, holdTime, fxTime, channel, szMessage);
}

void FF_UTIL_HudMessageFormat(CBasePlayer *pToPlayer, float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszFormat, ...)
{
	char szMessage[512];
	va_list argptr;
	va_start(argptr, pszFormat);
	Q_vsnprintf(szMessage, sizeof(szMessage), pszFormat, argptr);
	va_end(argptr);

	FF_UTIL_HudMessage(pToPlayer, x, y, effect, color1, color2, fadeInTime, fadeOutTime, holdTime, fxTime, channel, szMessage);
}

void FF_UTIL_HudMessage(CBasePlayer *pToPlayer, float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszMessage)
{
	CRecipientFilter filter;

	if( pToPlayer )
		filter.AddRecipient( pToPlayer );
	else
		filter.AddAllPlayers();

	FF_UTIL_HudMessage(filter, x, y, effect, color1, color2, fadeInTime, fadeOutTime, holdTime, fxTime, channel, pszMessage);
}

void FF_UTIL_HudMessage(CRecipientFilter &filter, float x, float y, HUD_MSG_EFFECT effect, color32 color1, color32 color2, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, int channel, const char *pszMessage)
{
	hudtextparms_s params;
	params.x		= x;
	params.y		= y;
	params.effect	= effect;
	params.r1		= color1.r;
	params.g1		= color1.g;
	params.b1		= color1.b;
	params.a1		= color1.a;
	params.r2		= color2.r;
	params.g2		= color2.g;
	params.b2		= color2.b;
	params.a2		= color2.a;
	params.fadeinTime	= fadeInTime;
	params.fadeoutTime	= fadeOutTime;
	params.holdTime	= holdTime;
	params.fxTime	= fxTime;
	params.channel	= channel;

	UTIL_HudMessage(filter, params, pszMessage);
}
