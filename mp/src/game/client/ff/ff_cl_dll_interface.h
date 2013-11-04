#ifndef FF_CL_DLL_INTERFACE_H
#define FF_CL_DLL_INTERFACE_H
#pragma once

#include "cdll_client_int.h"

class CHLClient;

//-----------------------------------------------------------------------------
// Purpose: engine to client .dll interface
//-----------------------------------------------------------------------------
class CFF_CL_DLL_Interface : public CHLClient
{
public:
	// squeek: I couldn't get DECLARE_CLASS_NOBASE to compile in CHLClient, so I couldn't use DECLARE_CLASS here
	// this works but it could potentially have negative consequences
	DECLARE_CLASS_GAMEROOT( CFF_CL_DLL_Interface, CHLClient );

	// <-- Extended functions (meaning the BaseClass function is always called)
	virtual int						Init( CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase *pGlobals );
	virtual void					Shutdown( void );
	
	virtual void					LevelInitPreEntity( const char *pMapName );
	virtual void					LevelInitPostEntity();
	virtual void					LevelShutdown( void );
	// --> Extended functions
	
	/*
	virtual void					PostInit();

	virtual bool					ReplayInit( CreateInterfaceFn fnReplayFactory );
	virtual bool					ReplayPostInit();

	virtual ClientClass				*GetAllClasses( void );

	virtual int						HudVidInit( void );
	virtual void					HudProcessInput( bool bActive );
	virtual void					HudUpdate( bool bActive );
	virtual void					HudReset( void );
	virtual void					HudText( const char * message );

	// Mouse Input Interfaces
	virtual void					IN_ActivateMouse( void );
	virtual void					IN_DeactivateMouse( void );
	virtual void					IN_Accumulate( void );
	virtual void					IN_ClearStates( void );
	virtual bool					IN_IsKeyDown( const char *name, bool& isdown );
	virtual void					IN_OnMouseWheeled( int nDelta );
	// Raw signal
	virtual int						IN_KeyEvent( int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding );
	virtual void					IN_SetSampleTime( float frametime );
	// Create movement command
	virtual void					CreateMove ( int sequence_number, float input_sample_frametime, bool active );
	virtual void					ExtraMouseSample( float frametime, bool active );
	virtual bool					WriteUsercmdDeltaToBuffer( bf_write *buf, int from, int to, bool isnewcommand );	
	virtual void					EncodeUserCmdToBuffer( bf_write& buf, int slot );
	virtual void					DecodeUserCmdFromBuffer( bf_read& buf, int slot );


	virtual void					View_Render( vrect_t *rect );
	virtual void					RenderView( const CViewSetup &view, int nClearFlags, int whatToDraw );
	virtual void					View_Fade( ScreenFade_t *pSF );
	
	virtual void					SetCrosshairAngle( const QAngle& angle );

	virtual void					InitSprite( CEngineSprite *pSprite, const char *loadname );
	virtual void					ShutdownSprite( CEngineSprite *pSprite );

	virtual int						GetSpriteSize( void ) const;

	virtual void					VoiceStatus( int entindex, qboolean bTalking );

	virtual void					InstallStringTableCallback( const char *tableName );

	virtual void					FrameStageNotify( ClientFrameStage_t curStage );

	virtual bool					DispatchUserMessage( int msg_type, bf_read &msg_data );

	// Save/restore system hooks
	virtual CSaveRestoreData  *SaveInit( int size );
	virtual void			SaveWriteFields( CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *, int );
	virtual void			SaveReadFields( CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *, int );
	virtual void			PreSave( CSaveRestoreData * );
	virtual void			Save( CSaveRestoreData * );
	virtual void			WriteSaveHeaders( CSaveRestoreData * );
	virtual void			ReadRestoreHeaders( CSaveRestoreData * );
	virtual void			Restore( CSaveRestoreData *, bool );
	virtual void			DispatchOnRestore();
	virtual void			WriteSaveGameScreenshot( const char *pFilename );

	// Given a list of "S(wavname) S(wavname2)" tokens, look up the localized text and emit
	//  the appropriate close caption if running with closecaption = 1
	virtual void			EmitSentenceCloseCaption( char const *tokenstream );
	virtual void			EmitCloseCaption( char const *captionname, float duration );

	virtual CStandardRecvProxies* GetStandardRecvProxies();

	virtual bool			CanRecordDemo( char *errorMsg, int length ) const;

	virtual void			OnDemoRecordStart( char const* pDemoBaseName );
	virtual void			OnDemoRecordStop();
	virtual void			OnDemoPlaybackStart( char const* pDemoBaseName );
	virtual void			OnDemoPlaybackStop();

	virtual bool			ShouldDrawDropdownConsole();

	// Get client screen dimensions
	virtual int				GetScreenWidth();
	virtual int				GetScreenHeight();

	// save game screenshot writing
	virtual void			WriteSaveGameScreenshotOfSize( const char *pFilename, int width, int height, bool bCreatePowerOf2Padded, bool bWriteVTF );

	// Gets the location of the player viewpoint
	virtual bool			GetPlayerView( CViewSetup &playerView );

	// Matchmaking
	virtual void			SetupGameProperties( CUtlVector< XUSER_CONTEXT > &contexts, CUtlVector< XUSER_PROPERTY > &properties );
	virtual uint			GetPresenceID( const char *pIDName );
	virtual const char		*GetPropertyIdString( const uint id );
	virtual void			GetPropertyDisplayString( uint id, uint value, char *pOutput, int nBytes );
	virtual void			StartStatsReporting( HANDLE handle, bool bArbitrated );

	virtual void			InvalidateMdlCache();

	virtual void			ReloadFilesInList( IFileList *pFilesToReload );

	// Let the client handle UI toggle - if this function returns false, the UI will toggle, otherwise it will not.
	virtual bool			HandleUiToggle();

	// Allow the console to be shown?
	virtual bool			ShouldAllowConsole();

	// Get renamed recv tables
	virtual CRenamedRecvTableInfo	*GetRenamedRecvTableInfos();

	// Get the mouthinfo for the sound being played inside UI panels
	virtual CMouthInfo		*GetClientUIMouthInfo();

	// Notify the client that a file has been received from the game server
	virtual void			FileReceived( const char * fileName, unsigned int transferID );

	virtual const char* TranslateEffectForVisionFilter( const char *pchEffectType, const char *pchEffectName );
	
	virtual void			ClientAdjustStartSoundParams( struct StartSoundParams_t& params );
	
	// Returns true if the disconnect command has been handled by the client
	virtual bool DisconnectAttempt( void );
public:
	void PrecacheMaterial( const char *pMaterialName );

	virtual bool IsConnectedUserInfoChangeAllowed( IConVar *pCvar );

private:
	void UncacheAllMaterials( );
	void ResetStringTablePointers();

	CUtlVector< IMaterial * > m_CachedMaterials;
	*/
};

#endif