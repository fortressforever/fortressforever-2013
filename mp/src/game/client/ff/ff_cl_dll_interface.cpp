#include "cbase.h"
#include "ff_cl_dll_interface.h"

#include "steam/steam_api.h"
#include "ff_sh_scriptman.h"

#define CLIENT_DLL_INTERFACE_VERSION		"VClient017"

int CFF_CL_DLL_Interface::Init( CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase *pGlobals )
{
	int ret = BaseClass::Init( appSystemFactory, physicsFactory, pGlobals );

	// start the Lua VM
	g_UIScriptManager.Init();
	g_GameScriptManager.Init();
	
	// Test steam API
	uint32 appId = steamapicontext->SteamUtils()->GetAppID();
	char msg[128];
	Q_snprintf(msg, 128,"Steam AppID = %i\n", appId);
	DevMsg(msg);
	/*
	// careful: you might want to use a long long if you're a internet celeb like squeek 
	int numFriends = steamapicontext->SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
	for (int i = 0; i < numFriends; i++)
	{
		CSteamID steamFrendzor = steamapicontext->SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
		AccountID_t acctId = steamFrendzor.GetAccountID();
		//const char *steamName = steamapicontext->SteamFriends()->GetFriendPersonaName(steamFrendzor);		
		if (acctId == 5800795)
		{
			//5800795 is my old steam acct running on my laptop to test this shit
			steamapicontext->SteamFriends()->InviteUserToGame(steamFrendzor, "connect 1.2.3.4;password barf");
		}

	}*/

	return ret;
}

void CFF_CL_DLL_Interface::Shutdown( void )
{
	// close the Lua VM
	g_UIScriptManager.Shutdown();
	g_GameScriptManager.Shutdown();

	BaseClass::Shutdown();
}

CFF_CL_DLL_Interface gFFClient;
IBaseClientDLL *clientdll = &gFFClient;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CFF_CL_DLL_Interface, IBaseClientDLL, CLIENT_DLL_INTERFACE_VERSION, gFFClient );