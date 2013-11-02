#include "cbase.h"
#include "ff_sv_dll_interface.h"

#include "ff_sh_scriptman.h"

bool CFF_SV_DLL_Interface::DLLInit(CreateInterfaceFn engineFactory, CreateInterfaceFn physicsFactory, CreateInterfaceFn fileSystemFactory, CGlobalVars *pGlobals)
{
	bool ret = BaseClass::DLLInit( engineFactory, physicsFactory, fileSystemFactory, pGlobals );
	
	// start the Lua VM
	g_GameScriptManager.Init();

	return ret;
}

void CFF_SV_DLL_Interface::DLLShutdown( void )
{
	// close the Lua VM
	g_GameScriptManager.Shutdown();

	BaseClass::DLLShutdown();
}

/** Sets player limits, required implementation
*/
void CServerGameClients::GetPlayerLimits( int& minplayers, int& maxplayers, int &defaultMaxPlayers ) const
{
	minplayers = 2; 
	maxplayers = MAX_PLAYERS; // TODO: currently 33
	defaultMaxPlayers = 16;
}

/** Called after all entities are loaded, required implementation
*/
void CServerGameDLL::LevelInit_ParseAllEntities( const char *pMapEntities )
{
	// FF TODO: FF map entity parse for stuff to keep around after reset
}

#define INTERFACEVERSION_SERVERGAMEDLL_VERSION_8	"ServerGameDLL008"
#define INTERFACEVERSION_SERVERGAMEDLL				"ServerGameDLL009"
#define INTERFACEVERSION_SERVERGAMEDLL_INT			9

CFF_SV_DLL_Interface g_ServerGameDLL;
// INTERFACEVERSION_SERVERGAMEDLL_VERSION_8 is compatible with the latest since we're only adding things to the end, so expose that as well.
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CFF_SV_DLL_Interface, IServerGameDLL008, INTERFACEVERSION_SERVERGAMEDLL_VERSION_8, g_ServerGameDLL );
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CFF_SV_DLL_Interface, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL, g_ServerGameDLL);

// When bumping the version to this interface, check that our assumption is still valid and expose the older version in the same way
COMPILE_TIME_ASSERT( INTERFACEVERSION_SERVERGAMEDLL_INT == 9 );