#ifndef FF_SV_DLL_INTERFACE_H
#define FF_SV_DLL_INTERFACE_H
#pragma once

#include "gameinterface.h"

class CFF_SV_DLL_Interface : public CServerGameDLL
{
	// squeek: I couldn't get DECLARE_CLASS_NOBASE to compile in CServerGameDLL, so I couldn't use DECLARE_CLASS here
	// this works but it could potentially have negative consequences
	DECLARE_CLASS_GAMEROOT( CFF_SV_DLL_Interface, CServerGameDLL );
	
	// <-- Extended functions (meaning the BaseClass function is always called)
	virtual bool			DLLInit(CreateInterfaceFn engineFactory, CreateInterfaceFn physicsFactory, CreateInterfaceFn fileSystemFactory, CGlobalVars *pGlobals);
	virtual void			DLLShutdown( void );
	// --> Extended functions

	/*
public:
	virtual bool			DLLInit(CreateInterfaceFn engineFactory, CreateInterfaceFn physicsFactory, 
										CreateInterfaceFn fileSystemFactory, CGlobalVars *pGlobals);
	virtual void			DLLShutdown( void );
	// Get the simulation interval (must be compiled with identical values into both client and game .dll for MOD!!!)
	virtual bool			ReplayInit( CreateInterfaceFn fnReplayFactory );
	virtual float			GetTickInterval( void ) const;
	virtual bool			GameInit( void );
	virtual void			GameShutdown( void );
	virtual bool			LevelInit( const char *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			LevelShutdown( void );
	virtual void			GameFrame( bool simulating ); // could be called multiple times before sending data to clients
	virtual void			PreClientUpdate( bool simulating ); // called after all GameFrame() calls, before sending data to clients

	virtual ServerClass*	GetAllServerClasses( void );
	virtual const char     *GetGameDescription( void );      
	virtual void			CreateNetworkStringTables( void );
	
	// Save/restore system hooks
	virtual CSaveRestoreData  *SaveInit( int size );
	virtual void			SaveWriteFields( CSaveRestoreData *, char const* , void *, datamap_t *, typedescription_t *, int );
	virtual void			SaveReadFields( CSaveRestoreData *, char const* , void *, datamap_t *, typedescription_t *, int );
	virtual void			SaveGlobalState( CSaveRestoreData * );
	virtual void			RestoreGlobalState( CSaveRestoreData * );
	virtual int				CreateEntityTransitionList( CSaveRestoreData *, int );
	virtual void			BuildAdjacentMapList( void );

	virtual void			PreSave( CSaveRestoreData * );
	virtual void			Save( CSaveRestoreData * );
	virtual void			GetSaveComment( char *comment, int maxlength, float flMinutes, float flSeconds, bool bNoTime = false );
#ifdef _XBOX
	virtual void			GetTitleName( const char *pMapName, char* pTitleBuff, int titleBuffSize );
#endif
	virtual void			WriteSaveHeaders( CSaveRestoreData * );

	virtual void			ReadRestoreHeaders( CSaveRestoreData * );
	virtual void			Restore( CSaveRestoreData *, bool );
	virtual bool			IsRestoring();

	// Retrieve info needed for parsing the specified user message
	virtual bool			GetUserMessageInfo( int msg_type, char *name, int maxnamelength, int& size );

	virtual CStandardSendProxies*	GetStandardSendProxies();

	virtual void			PostInit();
	virtual void			Think( bool finalTick );

	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );

	virtual void			PreSaveGameLoaded( char const *pSaveName, bool bInGame );

	// Returns true if the game DLL wants the server not to be made public.
	// Used by commentary system to hide multiplayer commentary servers from the master.
	virtual bool			ShouldHideServer( void );

	virtual void			InvalidateMdlCache();

	virtual void			SetServerHibernation( bool bHibernating );

	float	m_fAutoSaveDangerousTime;
	float	m_fAutoSaveDangerousMinHealthToCommit;
	bool	m_bIsHibernating;

	// Called after the steam API has been activated post-level startup
	virtual void			GameServerSteamAPIActivated( void );

	// Called after the steam API has been shutdown post-level startup
	virtual void			GameServerSteamAPIShutdown( void );

	// interface to the new GC based lobby system
	virtual IServerGCLobby *GetServerGCLobby() OVERRIDE;

	virtual const char *GetServerBrowserMapOverride() OVERRIDE;
	virtual const char *GetServerBrowserGameData() OVERRIDE;

private:

	// This can just be a wrapper on MapEntity_ParseAllEntities, but CS does some tricks in here
	// with the entity list.
	void LevelInit_ParseAllEntities( const char *pMapEntities );
	void LoadMessageOfTheDay();
	void LoadSpecificMOTDMsg( const ConVar &convar, const char *pszStringName );
	*/
};

extern CFF_SV_DLL_Interface g_ServerGameDLL;

#endif