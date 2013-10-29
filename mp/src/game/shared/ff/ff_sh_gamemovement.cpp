// standard headers
#include "cbase.h"
#include "ff_sh_gamemovement.h"
// necessary headers
#include "in_buttons.h"
#include "movevars_shared.h"
#include "rumble_shared.h"
#include "ff_sh_gamerules.h"

#ifdef CLIENT_DLL
	#include "ff_cl_player.h"
#else
	#include "ff_sv_player.h"
#endif

extern bool g_bMovementOptimizations;

//static ConVar ffdev_headcrush_damage("ffdev_headcrush_damage", "108", FCVAR_FF_FFDEV_REPLICATED, "Straight headcrush damage; not used if usefalldamage is on");
#define HEADCRUSH_DAMAGE 108.0f
//static ConVar ffdev_headcrush_usefalldamage("ffdev_headcrush_usefalldamage", "4.0", FCVAR_FF_FFDEV_REPLICATED, "0 = off, > 0 means take FALLDAMAGE * val damage");
#define HEADCRUSH_USEFALLDAMAGE 4.0f

//static ConVar sv_trimpmultiplier("sv_trimpmultiplier", "1.4", FCVAR_REPLICATED | FCVAR_CHEAT);
#define SV_TRIMPMULTIPLIER 1.4f
//static ConVar sv_trimpdownmultiplier("sv_trimpdownmultiplier", "1.2", FCVAR_REPLICATED | FCVAR_CHEAT);
#define SV_TRIMPDOWNMULTIPLIER 1.2f
//static ConVar sv_trimpmax("sv_trimpmax", "5000", FCVAR_REPLICATED);
#define SV_TRIMPMAX 5000.0f
//static ConVar sv_trimptriggerspeed("sv_trimptriggerspeed", "550", FCVAR_REPLICATED | FCVAR_CHEAT);
#define SV_TRIMPTRIGGERSPEED 550.0f
//static ConVar sv_trimptriggerspeeddown("sv_trimptriggerspeeddown", "50", FCVAR_REPLICATED | FCVAR_CHEAT);
#define SV_TRIMPTRIGGERSPEEDDOWN 50.0f

//static ConVar bhop_cap_soft("ffdev_bhop_cap_soft", "1.4", FCVAR_FF_FFDEV_REPLICATED); // bhop_cap_soft.GetFloat()
#define BHOP_CAP_SOFT 1.4f // also defined in ff_hud_speedometer - change it there too! 
//static ConVar bhop_cap_hard("ffdev_bhop_cap_hard", "2.0", FCVAR_FF_FFDEV_REPLICATED); // bhop_cap_hard.GetFloat()
#define BHOP_CAP_HARD 1.8f // also defined in ff_hud_speedometer - change it there too!
//static ConVar bhop_pcfactor("ffdev_bhop_pcfactor", "0.65", FCVAR_FF_FFDEV_REPLICATED); // bhop_pcfactor.GetFloat()
#define BHOP_PCFACTOR 0.65 

#ifdef CLIENT_DLL
ConVar cl_jumpqueue( "cl_jumpqueue", "0.0", FCVAR_ARCHIVE | FCVAR_USERINFO, "Enables jump queue (have to let go and press jump in between concurrent jumps) if set to 1" );
#endif

// debug purposes
void CFF_SH_GameMovement::PlayerMove( void )
{
#ifdef GAME_DLL
	static float flLastSurfaceFriction = -1.0f;
	if (player->m_surfaceFriction != flLastSurfaceFriction)
	{
		DevMsg("m_surfaceFriction changed from %f to %f\n", flLastSurfaceFriction, player->m_surfaceFriction);
		flLastSurfaceFriction = player->m_surfaceFriction;
	}
#endif

	BaseClass::PlayerMove();
}

bool CFF_SH_GameMovement::CheckJumpButton( void )
{
	// FF --> Use the base class in the case of a non-FFPlayer object
	CFF_SH_Player *pFFPlayer = ToFFPlayer( player );
	if (!pFFPlayer)
	{
		return BaseClass::CheckJumpButton();
	}
	// FF <--

	if (player->pl.deadflag)
	{
		mv->m_nOldButtons |= IN_JUMP ;	// don't jump again until released
		return false;
	}

	// See if we are waterjumping.  If so, decrement count and return.
	if (player->m_flWaterJumpTime)
	{
		player->m_flWaterJumpTime -= gpGlobals->frametime;
		if (player->m_flWaterJumpTime < 0)
			player->m_flWaterJumpTime = 0;
		
		return false;
	}

	// If we are in the water most of the way...
	if ( player->GetWaterLevel() >= WL_Waist )
	{	
		// swimming, not jumping
		SetGroundEntity( NULL );

		if(player->GetWaterType() == CONTENTS_WATER)    // We move up a certain amount
			mv->m_vecVelocity[2] = 100;
		else if (player->GetWaterType() == CONTENTS_SLIME)
			mv->m_vecVelocity[2] = 80;
		
		// play swiming sound
		if ( player->m_flSwimSoundTime <= 0 )
		{
			// Don't play sound again for 1 second
			player->m_flSwimSoundTime = 1000;
			PlaySwimSound();
		}

		return false;
	}
	
	// No more effect
 	if (player->GetGroundEntity() == NULL)
	{
		// FF --> Allow holding jump in the air to make you jump when you land
		if(!(mv->m_nOldButtons & IN_JUMP))
		{
			// FF Port Note: For only allowing holding jump while in the downward part of a jump, do a (mv->m_vecVelocity[2] < 0.0f) check here
			mv->m_nButtons &= ~IN_JUMP;
		}
		// FF <--
		return false;		// in air, so no effect
	}

	// Don't allow jumping when the player is in a stasis field.
#ifndef HL2_EPISODIC
	if ( player->m_Local.m_bSlowMovement )
		return false;
#endif

#ifdef CLIENT_DLL
	if ( mv->m_nOldButtons & IN_JUMP && cl_jumpqueue.GetBool() )
		return false;		// don't pogo stick
#else
	if ( mv->m_nOldButtons & IN_JUMP && (Q_atoi( engine->GetClientConVarValue( player->entindex(), "cl_jumpqueue" ) ) ) )
		return false;		// don't pogo stick
#endif

	// In the air now.
    SetGroundEntity( NULL );
	
	// FF TODO: Jump sound, unported code below
	//pFFPlayer->PlayJumpSound(mv->m_vecAbsOrigin, player->m_pSurfaceData, 1.0);
	player->PlayStepSound( (Vector &)mv->GetAbsOrigin(), player->m_pSurfaceData, 1.0, true );
	
	// FF TODO: Unported code below, unsure if its still needed
	//pFFPlayer->DoAnimationEvent(PLAYERANIMEVENT_JUMP);
	MoveHelper()->PlayerSetAnimation( PLAYER_JUMP );
	
	float startz = mv->m_vecVelocity[2];

	// Formula: sqrt(2 * gravity * jump_height)
	float flJumpSpeed = 268.6261342; // FF: sqrt(2.0f * 800.0f * 45.1f);

	if (player->m_pSurfaceData)
	{
		flJumpSpeed *= player->m_pSurfaceData->game.jumpFactor; 
	}
	
	// instead of altering horizontal velocity directly, alter this and then it will alter velocity at the end
	float flSpeed = sqrt(mv->m_vecVelocity[0] * mv->m_vecVelocity[0] + mv->m_vecVelocity[1] * mv->m_vecVelocity[1]);
	// normalize the horizontal velocity so we can just multiply flSpeed back in later
	if (flSpeed > 0)
	{
		mv->m_vecVelocity[0] /= flSpeed;
		mv->m_vecVelocity[1] /= flSpeed;
	}

/*
#ifdef GAME_DLL
	// FF TODO: Mancannon
	if ( ffplayer->m_flMancannonTime + 0.5f <= gpGlobals->curtime )
	{
#endif
*/
	ApplySoftCap( flSpeed );
/*
#ifdef GAME_DLL
	// FF TODO: Mancannon
	}
#endif
*/
	
	trace_t traceGround;
	// FF TODO: More reliable way to get the ground's normal? Can we use the old GroundEntity?
	// FF TODO: TFCBBOX
	// Adjusted for TFC bboxes.
	Vector vecStart = mv->GetAbsOrigin(); // + Vector(0, 0, GetPlayerMins()[2] + 1.0f);
	Vector vecStop = vecStart - Vector(0, 0, 60.0f);
	
	TracePlayerBBox(vecStart, vecStop, MASK_PLAYERSOLID, COLLISION_GROUP_PLAYER_MOVEMENT, traceGround);
	
	// we should always be able to find the ground
	Assert(traceGround.fraction != 1.0f);

	Vector vecNormalizedHorizVelocity( mv->m_vecVelocity[0], mv->m_vecVelocity[1], 0.0f );
	float flGroundDotProduct = DotProduct( vecNormalizedHorizVelocity, traceGround.plane.normal );

	bool bTrimped = DoTrimp( flGroundDotProduct, flSpeed, flJumpSpeed );

	bool bDownTrimped = false;
	if (!bTrimped)
	{
		bDownTrimped = DoDownTrimp( flGroundDotProduct, flSpeed, flJumpSpeed );
/*
#ifdef GAME_DLL
		// FF TODO: Mancannon
		if ( ffplayer->m_flMancannonTime + 0.5f <= gpGlobals->curtime )
		{
#endif
*/
		ApplyHardCap( flSpeed );
/*
#ifdef GAME_DLL
		// FF TODO: Mancannon
		}
#endif
*/
	}

	// FF: squeek: Note: Not checking for bTrimped before double jumping is what allows supertrimps
	DoDoubleJump( flJumpSpeed );
	
	// Set the horiz velocity to the new speed
	mv->m_vecVelocity[0] *= flSpeed;
	mv->m_vecVelocity[1] *= flSpeed;

	// Set the vertical velocity to jump speed
	mv->m_vecVelocity[2] = min( flJumpSpeed, SV_TRIMPMAX );

	FinishGravity();

	// FF TODO: Used in prediction error checking, all the definitions are in gamemovement.cpp
	//CheckV( player->CurrentCommandNumber(), "CheckJump", mv->m_vecVelocity );

	mv->m_outJumpVel.z += mv->m_vecVelocity[2] - startz;
	mv->m_outStepHeight += 0.15f;

	OnJump(mv->m_outJumpVel.z);

	// Flag that we jumped.
	mv->m_nOldButtons |= IN_JUMP;	// don't jump again until released
	return true;
}


float CFF_SH_GameMovement::ApplySoftCap( float &flSpeed )
{
	const float flSoftCapSpeed = BHOP_CAP_SOFT * mv->m_flMaxSpeed;
	float multi = 1.0f;

	if (flSpeed > flSoftCapSpeed)
	{
		float applied_cap = (flSpeed - flSoftCapSpeed) * BHOP_PCFACTOR + flSoftCapSpeed;
		multi = applied_cap / flSpeed;

		flSpeed *= multi;

		Assert(multi <= 1.0f);

#ifdef CLIENT_DLL
		DevMsg("[C] SoftCap\n");
#else
		DevMsg("[S] SoftCap\n");
#endif
	}

	return multi;
}
float CFF_SH_GameMovement::ApplyHardCap( float &flSpeed )
{
	const float flHardCapSpeed = BHOP_CAP_HARD * mv->m_flMaxSpeed;
	float multi = 1.0f;

	if (flSpeed > flHardCapSpeed)
	{
		multi = flHardCapSpeed / flSpeed;

		flSpeed *= multi;

		Assert(multi <= 1.0f);
		
#ifdef CLIENT_DLL
		DevMsg("[C] HardCap\n");
#else
		DevMsg("[S] HardCap\n");
#endif
	}

	return multi;
}
bool CFF_SH_GameMovement::DoTrimp( float flGroundDotProduct, float &flSpeed, float &flJumpSpeed )
{
	// Don't do anything for flat ground or downwardly sloping (relative to motion)
	// Changed to 0.15f to make it a bit less trimpy on only slightly uneven ground
	if (flSpeed > SV_TRIMPTRIGGERSPEED && flGroundDotProduct < -0.15f)
	{
   		flJumpSpeed += -flGroundDotProduct * flSpeed * SV_TRIMPMULTIPLIER;

		if (SV_TRIMPMULTIPLIER > 0)
		{
			flSpeed *= (1.0f / SV_TRIMPMULTIPLIER);
		}
		
#ifdef CLIENT_DLL
		DevMsg("[C] Trimp\n");
#else
		DevMsg("[S] Trimp\n");
#endif

		return true;
	}
	return false;
}
bool CFF_SH_GameMovement::DoDownTrimp( float flGroundDotProduct, float &flSpeed, float &flJumpSpeed )
{
	// AfterShock: travelling downwards onto a downward ramp - give boost horizontally
	if (flSpeed > SV_TRIMPTRIGGERSPEEDDOWN && flGroundDotProduct > 0.15f)
	{
		flSpeed *= SV_TRIMPDOWNMULTIPLIER;

		if (SV_TRIMPDOWNMULTIPLIER > 0)
		{
			flJumpSpeed *= (1.0f / SV_TRIMPDOWNMULTIPLIER);
		}
		
#ifdef CLIENT_DLL
		DevMsg("[C] DownTrimp\n");
#else
		DevMsg("[S] DownTrimp\n");
#endif

		return true;
	}
	return false;
}
bool CFF_SH_GameMovement::DoDoubleJump( float &flJumpSpeed )
{
	CFF_SH_Player *pFFPlayer = ToFFPlayer(player);
	Assert(pFFPlayer);

	// FF TODO: Move this back into player
	static float m_flNextJumpTimeForDouble = 0.0f;

	bool bDidDoubleJump = false;
	// FF TODO: Port the stuff that is needed for this code
	/*
	// Double jump - but don't allow double jumps while building, please!
	if( ffplayer->m_bCanDoubleJump && !ffplayer->IsStaticBuilding() )
	{
	*/
		float flElapsed = m_flNextJumpTimeForDouble - gpGlobals->curtime;

		if (flElapsed > 0 && flElapsed < 0.4f)
		{
			// AfterShock: Add a set amount for a double jump (dont multiply)
			flJumpSpeed += 190.0f;

#ifdef GAME_DLL
			DevMsg("[S] Double jump\n");
#else
			DevMsg("[C] Double jump\n");
#endif
			bDidDoubleJump = true;
			// FF TODO: Port the stuff that is needed for this code
			//ffplayer->m_bCanDoubleJump = false;
		}

		m_flNextJumpTimeForDouble = gpGlobals->curtime + 0.5f;
	/*
	}
	*/
	return bDidDoubleJump;
}

void CFF_SH_GameMovement::OnLand( float flFallVelocity )
{
	BaseClass::OnLand( flFallVelocity );

#ifdef GAME_DLL
	if (player->GetGroundEntity() && player->GetGroundEntity()->IsPlayer())
	{
		CFF_SH_Player *pCrushedPlayer = ToFFPlayer(player->GetGroundEntity());

		if (pCrushedPlayer && pCrushedPlayer != player)
		{
			float flCrushDamage = 0.0f;
			if (HEADCRUSH_USEFALLDAMAGE > 0)
			{
				float flFallDamage = g_pGameRules->FlPlayerFallDamage( player );	
				flCrushDamage = flFallDamage * HEADCRUSH_USEFALLDAMAGE;
			}
			else
			{
				flCrushDamage = HEADCRUSH_DAMAGE;
			}
			if (flCrushDamage > 0)
			{
				// FF TODO: Reinstate custom killtype
				CTakeDamageInfo info( player, player, flCrushDamage, DMG_DIRECT/*, KILLTYPE_HEADCRUSH*/ );
				pCrushedPlayer->TakeDamage(info);
			}
		}
	}
#endif
}

void CFF_SH_GameMovement::PlayerRoughLandingEffects( float fvol )
{
	if ( fvol > 0.0 )
	{
		// FF TODO: Play fall sound here, unported code below
		/*
		// --> Mirv: Use a fall sound, and reduce the volume for spies
		CFFPlayer *pFFPlayer = ToFFPlayer(player);
		Assert( pFFPlayer );

		pFFPlayer->PlayFallSound( mv->m_vecAbsOrigin, player->m_pSurfaceData, fvol );

		//
		// Play landing sound right away.
		// #0000599: footstep continues to play after cratering
		// #0000401: Oddity with the fall sound.
		pFFPlayer->m_flStepSoundTime = gpGlobals->curtime + 0.600f;
		pFFPlayer->m_flJumpTime = gpGlobals->curtime + 0.600f;

		// Play step sound for current texture.
		//player->PlayStepSound( mv->m_vecAbsOrigin, player->m_pSurfaceData, fvol, true );

		// <-- Mirv: Use a fall sound, and reduce the volume for spies
		*/

		//
		// Play landing sound right away.
		player->m_flStepSoundTime = 400;

		// Play step sound for current texture.
		player->PlayStepSound( (Vector &)mv->GetAbsOrigin(), player->m_pSurfaceData, fvol, true );

		//
		// Knock the screen around a little bit, temporary effect.
		//
		player->m_Local.m_vecPunchAngle.Set( ROLL, player->m_Local.m_flFallVelocity * 0.013 );

		if ( player->m_Local.m_vecPunchAngle[PITCH] > 8 )
		{
			player->m_Local.m_vecPunchAngle.Set( PITCH, 8 );
		}

#if !defined( CLIENT_DLL )
		player->RumbleEffect( ( fvol > 0.85f ) ? ( RUMBLE_FALL_LONG ) : ( RUMBLE_FALL_SHORT ), 0, RUMBLE_FLAGS_NONE );
#endif
	}
}

void CFF_SH_GameMovement::FinishUnDuck( void )
{
	int i;
	trace_t trace;
	Vector newOrigin;

	VectorCopy( mv->GetAbsOrigin(), newOrigin );

	// FF -->
	// The extra check (m_Local.m_bDucked) added because players were popping up 
	// into the air when they hadn't yet been moved down for the duck
	if ( player->GetGroundEntity() != NULL && player->m_Local.m_bDucked )
	// FF <--
	{
		for ( i = 0; i < 3; i++ )
		{
			newOrigin[i] += ( VEC_DUCK_HULL_MIN_SCALED( player )[i] - VEC_HULL_MIN_SCALED( player )[i] );
		}
	}
	else
	{
		// If in air an letting go of crouch, make sure we can offset origin to make
		//  up for uncrouching
		Vector hullSizeNormal = VEC_HULL_MAX_SCALED( player ) - VEC_HULL_MIN_SCALED( player );
		Vector hullSizeCrouch = VEC_DUCK_HULL_MAX_SCALED( player ) - VEC_DUCK_HULL_MIN_SCALED( player );
		Vector viewDelta = ( hullSizeNormal - hullSizeCrouch ) / 2.0f;
		viewDelta.Negate();
		VectorAdd( newOrigin, viewDelta, newOrigin );
	}

	player->m_Local.m_bDucked = false;
	player->RemoveFlag( FL_DUCKING );
	player->m_Local.m_bDucking  = false;
	player->m_Local.m_bInDuckJump  = false;
	player->SetViewOffset( GetPlayerViewOffset( false ) );
	player->m_Local.m_flDucktime = 0;

	mv->SetAbsOrigin( newOrigin );

#ifdef CLIENT_DLL
	player->ResetLatched();
#endif

	// Recategorize position since ducking can change origin
	CategorizePosition();
	
	// FF TODO: A lot of unported changes here; this block replaces the else { } and below
	/*
	//trace = pmove->PM_PlayerTrace( newOrigin, newOrigin, PM_NORMAL, -1 );
	TracePlayerBBox(newOrigin, newOrigin, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace);

	if ( !trace.startsolid )
	{
		player->m_Local.m_bDucked = false; // pmove->usehull = 0;

		// Oh, no, changing hulls stuck us into something, try unsticking downward first.
		//trace = pmove->PM_PlayerTrace( newOrigin, newOrigin, PM_NORMAL, -1  );
		TracePlayerBBox(newOrigin, newOrigin, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace);

		if ( trace.startsolid )
		{
			// See if we are stuck?  If so, stay ducked with the duck hull until we have a clear spot
			//Con_Printf( "unstick got stuck\n" );
			player->m_Local.m_bDucked = true; // pmove->usehull = 1;
			return;
		}

		player->RemoveFlag(FL_DUCKING); // pmove->flags &= ~FL_DUCKING;
		player->m_Local.m_bDucking  = false;
		//pmove->view_ofs[2] = VEC_VIEW;
		Vector vecViewOffset = player->GetViewOffset();
		vecViewOffset[2] = VEC_VIEW[2];
		player->SetViewOffset(vecViewOffset);
		player->m_Local.m_flDucktime = 0;

		VectorCopy( newOrigin, mv->m_vecAbsOrigin );

		// Recatagorize position since ducking can change origin
		CategorizePosition();
	}
	*/
}

void CFF_SH_GameMovement::Duck( void )
{
	// FF TODO: Port these changes, this function was totally rewritten
	/*
	if (!player->IsAlive())
		return;

	int i;
	float time;
	float duckFraction;

	int buttonsChanged	= ( mv->m_nOldButtons ^ mv->m_nButtons );	// These buttons have changed this frame
	int nButtonPressed	=  buttonsChanged & mv->m_nButtons;		// The changed ones still down are "pressed"

	//int duckchange		= buttonsChanged & IN_DUCK ? 1 : 0;
	//int duckpressed		= nButtonPressed & IN_DUCK ? 1 : 0;

	if ( mv->m_nButtons & IN_DUCK )
	{
		mv->m_nOldButtons |= IN_DUCK;
	}
	else
	{
		mv->m_nOldButtons &= ~IN_DUCK;
	}

	//if ( player->GetFlags() & FL_DUCKING )
	//{
	//	pmove->cmd.forwardmove *= 0.333;
	//	pmove->cmd.sidemove    *= 0.333;
	//	pmove->cmd.upmove      *= 0.333;
	//}

	HandleDuckingSpeedCrop();

	if ( ( mv->m_nButtons & IN_DUCK ) || ( player->m_Local.m_bDucking ) || ( player->GetFlags() & FL_DUCKING ) )
	{
		if ( mv->m_nButtons & IN_DUCK )
		{
			if ( (nButtonPressed & IN_DUCK ) && !( player->GetFlags() & FL_DUCKING ) )
			{
				// Use 1 second so super long jump will work
				player->m_Local.m_flDucktime = 1000;
				player->m_Local.m_bDucking    = true;
			}

			time = max( 0.0, ( 1.0 - (float)player->m_Local.m_flDucktime / 1000.0 ) );

			if ( player->m_Local.m_bDucking )
			{
				// Finish ducking immediately if duck time is over or not on ground
				if ( ( (float)player->m_Local.m_flDucktime / 1000.0 <= ( 1.0 - TIME_TO_DUCK ) ) ||
					( player->GetGroundEntity() == NULL ) && player->m_Local.m_flDucktime > 0 )
				{
					player->m_Local.m_bDucked = true; //pmove->usehull = 1;
					//pmove->view_ofs[2] = VEC_DUCK_VIEW;
					Vector vecOffset = player->GetViewOffset();
					vecOffset[2] = VEC_DUCK_VIEW[2];
					player->SetViewOffset(vecOffset);

					player->AddFlag(FL_DUCKING);	//player->GetFlags() |= FL_DUCKING;

					//player->m_Local.m_bDucking = false;

					// HACKHACK - Fudge for collision bug - no time to fix this properly
					if ( player->GetGroundEntity() != NULL )
					{
						trace_t pm;
						TracePlayerBBox(mv->m_vecAbsOrigin, mv->m_vecAbsOrigin - (VEC_DUCK_HULL_MIN - VEC_HULL_MIN), PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm);
						for ( i = 0; i < 3; i++ )
						{
							//Assert(pm.fraction == 1.0f);
							mv->m_vecAbsOrigin[i] -= pm.fraction * ( VEC_DUCK_HULL_MIN[i] - VEC_HULL_MIN[i] );
						}
						// See if we are stuck?
						FixPlayerCrouchStuck(true);

						// Recatagorize position since ducking can change origin
						CategorizePosition();
					}
					else
					{
						player->m_Local.m_bDucking = false;
					}
				}
				else
				{
					float fMore = VEC_DUCK_HULL_MIN[2] - VEC_HULL_MIN[2];

					// Calc parametric time
					duckFraction = SplineFraction( time, (1.0/TIME_TO_DUCK) );
					
					//pmove->view_ofs[2] = ((VEC_DUCK_VIEW - fMore ) * duckFraction) + (VEC_VIEW * (1-duckFraction));
					Vector vecViewOfs = player->GetViewOffset();
					vecViewOfs[2] = ((VEC_DUCK_VIEW[2] - fMore ) * duckFraction) + (VEC_VIEW[2] * (1-duckFraction));
					player->SetViewOffset(vecViewOfs);
				}
			}
		}
		else
		{
			// Try to unduck
			FinishUnDuck();
		}
	}
	*/

	int buttonsChanged	= ( mv->m_nOldButtons ^ mv->m_nButtons );	// These buttons have changed this frame
	int buttonsPressed	=  buttonsChanged & mv->m_nButtons;			// The changed ones still down are "pressed"
	int buttonsReleased	=  buttonsChanged & mv->m_nOldButtons;		// The changed ones which were previously down are "released"

	// Check to see if we are in the air.
	bool bInAir = ( player->GetGroundEntity() == NULL );
	bool bInDuck = ( player->GetFlags() & FL_DUCKING ) ? true : false;
	bool bDuckJump = ( player->m_Local.m_flJumpTime > 0.0f );
	bool bDuckJumpTime = ( player->m_Local.m_flDuckJumpTime > 0.0f );

	if ( mv->m_nButtons & IN_DUCK )
	{
		mv->m_nOldButtons |= IN_DUCK;
	}
	else
	{
		mv->m_nOldButtons &= ~IN_DUCK;
	}

	// Handle death.
	if ( IsDead() )
		return;

	// Slow down ducked players.
	HandleDuckingSpeedCrop();

	// If the player is holding down the duck button, the player is in duck transition, ducking, or duck-jumping.
	if ( ( mv->m_nButtons & IN_DUCK ) || player->m_Local.m_bDucking  || bInDuck || bDuckJump )
	{
		// DUCK
		if ( ( mv->m_nButtons & IN_DUCK ) || bDuckJump )
		{
/* FF: squeek: commenting this block out for now
// XBOX SERVER ONLY
#if !defined(CLIENT_DLL)
			if ( IsX360() && buttonsPressed & IN_DUCK )
			{
				// Hinting logic
				if ( player->GetToggledDuckState() && player->m_nNumCrouches < NUM_CROUCH_HINTS )
				{
					UTIL_HudHintText( player, "#Valve_Hint_Crouch" );
					player->m_nNumCrouches++;
				}
			}
#endif
*/
			// Have the duck button pressed, but the player currently isn't in the duck position.
			if ( ( buttonsPressed & IN_DUCK ) && !bInDuck && !bDuckJump && !bDuckJumpTime )
			{
				player->m_Local.m_flDucktime = GAMEMOVEMENT_DUCK_TIME;
				player->m_Local.m_bDucking = true;
			}
			
			// The player is in duck transition and not duck-jumping.
			if ( player->m_Local.m_bDucking && !bDuckJump && !bDuckJumpTime )
			{
				float flDuckMilliseconds = MAX( 0.0f, GAMEMOVEMENT_DUCK_TIME - ( float )player->m_Local.m_flDucktime );
				float flDuckSeconds = flDuckMilliseconds * 0.001f;
				
				// Finish in duck transition when transition time is over, in "duck", in air.
				if ( ( flDuckSeconds > TIME_TO_DUCK ) || bInDuck || bInAir )
				{
					FinishDuck();
				}
				else
				{
					// Calc parametric time
					float flDuckFraction = SimpleSpline( flDuckSeconds / TIME_TO_DUCK );
					SetDuckedEyeOffset( flDuckFraction );
				}
			}

			if ( bDuckJump )
			{
				// Make the bounding box small immediately.
				if ( !bInDuck )
				{
					StartUnDuckJump();
				}
				else
				{
					// Check for a crouch override.
					if ( !( mv->m_nButtons & IN_DUCK ) )
					{
						trace_t trace;
						if ( CanUnDuckJump( trace ) )
						{
							FinishUnDuckJump( trace );
							player->m_Local.m_flDuckJumpTime = ( GAMEMOVEMENT_TIME_TO_UNDUCK * ( 1.0f - trace.fraction ) ) + GAMEMOVEMENT_TIME_TO_UNDUCK_INV;
						}
					}
				}
			}
		}
		// UNDUCK (or attempt to...)
		else
		{
			if ( player->m_Local.m_bInDuckJump )
			{
				// Check for a crouch override.
   				if ( !( mv->m_nButtons & IN_DUCK ) )
				{
					trace_t trace;
					if ( CanUnDuckJump( trace ) )
					{
						FinishUnDuckJump( trace );
					
						if ( trace.fraction < 1.0f )
						{
							player->m_Local.m_flDuckJumpTime = ( GAMEMOVEMENT_TIME_TO_UNDUCK * ( 1.0f - trace.fraction ) ) + GAMEMOVEMENT_TIME_TO_UNDUCK_INV;
						}
					}
				}
				else
				{
					player->m_Local.m_bInDuckJump = false;
				}
			}

			if ( bDuckJumpTime )
				return;

			// Try to unduck unless automovement is not allowed
			// NOTE: When not onground, you can always unduck
			if ( player->m_Local.m_bAllowAutoMovement || bInAir || player->m_Local.m_bDucking )
			{
				// We released the duck button, we aren't in "duck" and we are not in the air - start unduck transition.
				if ( ( buttonsReleased & IN_DUCK ) )
				{
					if ( bInDuck && !bDuckJump )
					{
						player->m_Local.m_flDucktime = GAMEMOVEMENT_DUCK_TIME;
					}
					else if ( player->m_Local.m_bDucking && !player->m_Local.m_bDucked )
					{
						// Invert time if release before fully ducked!!!
						float unduckMilliseconds = 1000.0f * TIME_TO_UNDUCK;
						float duckMilliseconds = 1000.0f * TIME_TO_DUCK;
						float elapsedMilliseconds = GAMEMOVEMENT_DUCK_TIME - player->m_Local.m_flDucktime;

						float fracDucked = elapsedMilliseconds / duckMilliseconds;
						float remainingUnduckMilliseconds = fracDucked * unduckMilliseconds;

						player->m_Local.m_flDucktime = GAMEMOVEMENT_DUCK_TIME - unduckMilliseconds + remainingUnduckMilliseconds;
					}
				}
				

				// Check to see if we are capable of unducking.
				if ( CanUnduck() )
				{
					// or unducking
					if ( ( player->m_Local.m_bDucking || player->m_Local.m_bDucked ) )
					{
						float flDuckMilliseconds = MAX( 0.0f, GAMEMOVEMENT_DUCK_TIME - (float)player->m_Local.m_flDucktime );
						float flDuckSeconds = flDuckMilliseconds * 0.001f;
						
						// Finish ducking immediately if duck time is over or not on ground
						if ( flDuckSeconds > TIME_TO_UNDUCK || ( bInAir && !bDuckJump ) )
						{
							FinishUnDuck();
						}
						else
						{
							// Calc parametric time
							float flDuckFraction = SimpleSpline( 1.0f - ( flDuckSeconds / TIME_TO_UNDUCK ) );
							SetDuckedEyeOffset( flDuckFraction );
							player->m_Local.m_bDucking = true;
						}
					}
				}
				else
				{
					// Still under something where we can't unduck, so make sure we reset this timer so
					//  that we'll unduck once we exit the tunnel, etc.
					if ( player->m_Local.m_flDucktime != GAMEMOVEMENT_DUCK_TIME )
					{
						SetDuckedEyeOffset(1.0f);
						player->m_Local.m_flDucktime = GAMEMOVEMENT_DUCK_TIME;
						player->m_Local.m_bDucked = true;
						player->m_Local.m_bDucking = false;
						player->AddFlag( FL_DUCKING );
					}
				}
			}
		}
	}
	// HACK: (jimd 5/25/2006) we have a reoccuring bug (#50063 in Tracker) where the player's
	// view height gets left at the ducked height while the player is standing, but we haven't
	// been  able to repro it to find the cause.  It may be fixed now due to a change I'm
	// also making in UpdateDuckJumpEyeOffset but just in case, this code will sense the 
	// problem and restore the eye to the proper position.  It doesn't smooth the transition,
	// but it is preferable to leaving the player's view too low.
	//
	// If the player is still alive and not an observer, check to make sure that
	// his view height is at the standing height.
	else if ( !IsDead() && !player->IsObserver() && !player->IsInAVehicle() )
	{
		if ( ( player->m_Local.m_flDuckJumpTime == 0.0f ) && ( fabs(player->GetViewOffset().z - GetPlayerViewOffset( false ).z) > 0.1 ) )
		{
			// we should rarely ever get here, so assert so a coder knows when it happens
			Assert(0);
			DevMsg( 1, "Restoring player view height\n" );

			// set the eye height to the non-ducked height
			SetDuckedEyeOffset(0.0f);
		}
	}
}

// Expose our interface.
static CFF_SH_GameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CFF_SH_GameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement );