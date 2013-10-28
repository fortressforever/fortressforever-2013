// standard headers
#include "cbase.h"
#include "ff_sh_gamemovement.h"
// necessary headers
#include "in_buttons.h"
#include "movevars_shared.h"
#include "rumble_shared.h"

// variables
ConVar sv_sharkingfriction("sv_sharkingfriction", "1", FCVAR_REPLICATED);
#define SV_SHARKINGFRICTION sv_sharkingfriction.GetFloat()

// debug purposes
void CFF_SH_GameMovement::PlayerMove( void )
{
	static float flLastSurfaceFriction = -1.0f;
	if (player->m_surfaceFriction != flLastSurfaceFriction)
	{
		DevMsg("m_surfaceFriction changed from %f to %f\n", flLastSurfaceFriction, player->m_surfaceFriction);
		flLastSurfaceFriction = player->m_surfaceFriction;
	}

	BaseClass::PlayerMove();
}

void CFF_SH_GameMovement::OnLand( float flFallVelocity )
{
	BaseClass::OnLand( flFallVelocity );

	// FF TODO: Rewrite this so that it doesn't use volume as a condition
	// FF TODO: Make this code shared if possible
	/*
	if (fvol == 1.0f)
	{
#ifdef GAME_DLL
		if (player->GetGroundEntity() && player->GetGroundEntity()->IsPlayer())
		{
			CFFPlayer *pCrushedPlayer = ToFFPlayer(player->GetGroundEntity());

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
				CTakeDamageInfo info( player, player, flCrushDamage, DMG_DIRECT, KILLTYPE_HEADCRUSH );
				pCrushedPlayer->TakeDamage(info);
			}
		}
#endif
	}
	*/
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
		Vector viewDelta = ( hullSizeNormal - hullSizeCrouch );
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