#include "cbase.h"
#include "ff_sh_gamemovement.h"

// variables
#include "ff_sh_vars_movement.h"
#include "movevars_shared.h"

// necessary headers
#include "in_buttons.h"
#include "rumble_shared.h"
#include "ff_sh_gamerules.h"
#ifdef CLIENT_DLL
	#include "ff_cl_player.h"
#else
	#include "ff_sv_player.h"
#endif

/** Prints any changes to m_surfaceFriction. Extended for temporary/debug purposes only.
	Overwritten from CGameMovement.
*/
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

/** Executes all FF-specific jump-related logic. 
	Overwritten from CGameMovement.

	@returns Whether or not a jump was performed
*/
bool CFF_SH_GameMovement::CheckJumpButton( void )
{
	// FF --> Use the base class in the case of a non-FFPlayer object
	CFF_SH_Player *pFFPlayer = ToFFPlayer( player );
	if (!pFFPlayer)
	{
		return BaseClass::CheckJumpButton();
	}
	// FF <--

	if (!CanJump())
		return false;

	CBaseEntity *pOldGroundEntity = player->GetGroundEntity();

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
	Assert(traceGround.m_pEnt == pOldGroundEntity);

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

/** Checks to see if the player is able to jump

	@returns Whether or not a jump is able to be performed
*/
bool CFF_SH_GameMovement::CanJump( void )
{
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

	bool bUseJumpQueue;
#ifdef CLIENT_DLL
	bUseJumpQueue = cl_jumpqueue.GetBool();
#else
	bUseJumpQueue = (Q_atoi( engine->GetClientConVarValue( player->entindex(), "cl_jumpqueue" ) ) ) != 0;
#endif
	
	// No more effect
 	if (player->GetGroundEntity() == NULL)
	{
		// FF --> Allow holding jump in the air to make you jump when you land
		if( bUseJumpQueue && !(mv->m_nOldButtons & IN_JUMP) )
		{
			// FF Port Note: For only allowing holding jump while in the downward part of a jump, do a (mv->m_vecVelocity[2] < 0.0f) check here
			// Jump wasn't held last frame, so act like jump isn't held for the rest of the airtime
			// FF TODO: Potentially add a jump-specific button-held-state variable to handle jump queueing, since manipulating the button states directly is rather hacky
			mv->m_nButtons &= ~IN_JUMP;
		}
		else if (!bUseJumpQueue)
		{
			// act like the jump key was never held last frame; this is needed because of how the trimping code interacts with the rampslide code
			mv->m_nOldButtons &= ~IN_JUMP;
		}
		// FF <--
		return false;		// in air, so no effect
	}
	
	// Don't allow jumping when the player is in a stasis field.
#ifndef HL2_EPISODIC
	if ( player->m_Local.m_bSlowMovement )
		return false;
#endif
	
	// If jump queue is enabled and we have been holding jump since we last jumped, then don't allow jumping again until it's been released for at least one frame
	if ( bUseJumpQueue && mv->m_nOldButtons & IN_JUMP )
		return false;		// don't pogo stick

	return true;
}

/** If flSpeed is above the soft cap, alters flSpeed accordingly

	@param flSpeed Horizontal speed of the player, altered directly
	@returns Multiplier used to alter flSpeed
*/
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
		DevMsg("[C] SoftCap from %f to %f\n", flSpeed/multi, flSpeed);
#else
		DevMsg("[S] SoftCap from %f to %f\n", flSpeed/multi, flSpeed);
#endif
	}

	return multi;
}

/** Sets flSpeed to the hard cap if flSpeed is > the hard cap

	@param flSpeed Horizontal speed of the player, altered directly
	@returns Multiplier used to alter flSpeed
*/
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
		DevMsg("[C] HardCap from %f to %f\n", flSpeed/multi, flSpeed);
#else
		DevMsg("[S] HardCap from %f to %f\n", flSpeed/multi, flSpeed);
#endif
	}

	return multi;
}

/** Performs a trimp only if it's currently possible

	@param flGroundDotProduct DotProduct of player's vertical velocity vector and their current ground surface's normal
	@param flSpeed Horizontal speed of the player, altered directly
	@param flJumpSpeed Vertical speed of the player, altered directly
	@returns Whether or not a trimp was performed
*/
bool CFF_SH_GameMovement::DoTrimp( float flGroundDotProduct, float &flSpeed, float &flJumpSpeed )
{
	// Don't do anything for flat ground or downwardly sloping (relative to motion)
	// Changed to 0.15f to make it a bit less trimpy on only slightly uneven ground
	if (flSpeed > SV_TRIMPTRIGGERSPEED && flGroundDotProduct < -0.15f)
	{
		float flOldJumpSpeed = flJumpSpeed;
		float flOldSpeed = flSpeed;

   		flJumpSpeed += -flGroundDotProduct * flSpeed * SV_TRIMPMULTIPLIER;

		if (SV_TRIMPMULTIPLIER > 0)
		{
			flSpeed *= (1.0f / SV_TRIMPMULTIPLIER);
		}
		
#ifdef CLIENT_DLL
		DevMsg("[C] Trimp from %f/%f to %f/%f (forwardspeed/jumpspeed)\n", flOldSpeed, flOldJumpSpeed, flSpeed, flJumpSpeed);
#else
		DevMsg("[S] Trimp from %f/%f to %f/%f (forwardspeed/jumpspeed)\n", flOldSpeed, flOldJumpSpeed, flSpeed, flJumpSpeed);
#endif

		return true;
	}
	return false;
}

/** Performs a downtrimp only if it's currently possible

	@param flGroundDotProduct DotProduct of player's vertical velocity vector and their current ground surface's normal
	@param flSpeed Horizontal speed of the player, altered directly
	@param flJumpSpeed Vertical speed of the player, altered directly
	@returns Whether or not a downtrimp was performed
*/
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
		DevMsg("[C] DownTrimp from %f/%f to %f/%f (forwardspeed/jumpspeed)\n", flSpeed/SV_TRIMPDOWNMULTIPLIER, flJumpSpeed/SV_TRIMPDOWNMULTIPLIER, flSpeed, flJumpSpeed);
#else
		DevMsg("[S] DownTrimp from %f/%f to %f/%f (forwardspeed/jumpspeed)\n", flSpeed/SV_TRIMPDOWNMULTIPLIER, flJumpSpeed/SV_TRIMPDOWNMULTIPLIER, flSpeed, flJumpSpeed);
#endif

		return true;
	}
	return false;
}

/** Performs a double jump only if it's currently possible

	@param flJumpSpeed Vertical speed of the player, altered directly
	@returns Whether or not a double jump was performed
*/
bool CFF_SH_GameMovement::DoDoubleJump( float &flJumpSpeed )
{
	CFF_SH_Player *pFFPlayer = ToFFPlayer(player);
	Assert(pFFPlayer);

	bool bDidDoubleJump = false;
	// FF TODO: Port the stuff that is needed for this code
	/*
	// Double jump - but don't allow double jumps while building, please!
	if( ffplayer->m_bCanDoubleJump && !ffplayer->IsStaticBuilding() )
	{
	*/
		float flElapsed = pFFPlayer->m_flNextJumpTimeForDouble - gpGlobals->curtime;

		if (flElapsed > 0 && flElapsed < 0.4f)
		{
			// AfterShock: Add a set amount for a double jump (dont multiply)
			flJumpSpeed += 190.0f;

#ifdef GAME_DLL
			DevMsg("[S] Double jump from %f to %f (jumpspeed)\n", flJumpSpeed-190.0f, flJumpSpeed);
#else
			DevMsg("[C] Double jump from %f to %f (jumpspeed)\n", flJumpSpeed-190.0f, flJumpSpeed);
#endif
			bDidDoubleJump = true;
			// FF TODO: Port the stuff that is needed for this code
			//ffplayer->m_bCanDoubleJump = false;
		}

		pFFPlayer->m_flNextJumpTimeForDouble = gpGlobals->curtime + 0.5f;
	/*
	}
	*/
	return bDidDoubleJump;
}

/** Handles landing after falling from any height
	Extended from CGameMovement

	Implements headcrushing
*/
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

/** Handles effects from landing after falling from any height
	Overwritten from CGameMovement

	@param fvol Volume of the effect sound (between 0 and 1)
*/
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

/** Handles ducking
	Overwritten from CGameMovement
	Purpose: Make unducking instantaneous
*/
void CFF_SH_GameMovement::Duck( void )
{
	int buttonsChanged	= ( mv->m_nOldButtons ^ mv->m_nButtons );	// These buttons have changed this frame
	int buttonsPressed	=  buttonsChanged & mv->m_nButtons;			// The changed ones still down are "pressed"
	//int buttonsReleased	=  buttonsChanged & mv->m_nOldButtons;		// The changed ones which were previously down are "released"

	// Check to see if we are in the air.
	bool bInAir = ( player->GetGroundEntity() == NULL );
	bool bInDuck = ( player->GetFlags() & FL_DUCKING ) ? true : false;

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
	if ( ( mv->m_nButtons & IN_DUCK ) || player->m_Local.m_bDucking  || bInDuck )
	{
		// DUCK
		if ( ( mv->m_nButtons & IN_DUCK ) )
		{
			// Have the duck button pressed, but the player currently isn't in the duck position.
			if ( ( buttonsPressed & IN_DUCK ) && !bInDuck )
			{
				player->m_Local.m_flDucktime = GAMEMOVEMENT_DUCK_TIME;
				player->m_Local.m_bDucking = true;
			}
			
			// The player is in duck transition and not duck-jumping.
			if ( player->m_Local.m_bDucking )
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
		}
		// UNDUCK (or attempt to...)
		else
		{
			// Try to unduck
			if (CanUnduck())
				FinishUnDuck();
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

/** Handles speed cropping while ducking
	Overwritten from CGameMovement
	Purpose: Allow for TFC-style duck speed cropping
*/
void CFF_SH_GameMovement::HandleDuckingSpeedCrop( void )
{
	// in HL1/TFC, crouching altered intended movement speed even while in the air
	// it basically makes you lose speed while air controlling at high speeds
	if (FFDEV_TFC_CROUCH_AIRMOVEMENT && ( player->GetFlags() & FL_DUCKING ))
	{
		float frac = 0.33333333f;
		mv->m_flForwardMove	*= frac;
		mv->m_flSideMove	*= frac;
		mv->m_flUpMove		*= frac;
		m_iSpeedCropped		|= SPEED_CROPPED_DUCK;
	}
	else
	{
		BaseClass::HandleDuckingSpeedCrop();
	}
}

// Expose our interface.
// Note: There can only be one IGameMovement exposed at a time
static CFF_SH_GameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CFF_SH_GameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement );