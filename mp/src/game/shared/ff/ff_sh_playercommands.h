#pragma once
#include "cbase.h"
#include "ff_sh_player.h"
#include <string>

#define FF_COMMAND(cmd, desc) \
	void CliCmdFunc_##cmd(const CCommand &args) \
	{ \
		std::string fullcmd; \
		for(int i = 0; i < args.ArgC(); i++) \
		{ \
			if(i > 0) \
			fullcmd += ' '; \
			fullcmd += args.Arg(i); \
		} \
		engine->ServerCmd(fullcmd.c_str()); \
	} \
	static ConCommand CliCmd_##cmd(#cmd, CliCmdFunc_##cmd, desc ); 

#ifdef CLIENT_DLL
FF_COMMAND(team, "join a team");
FF_COMMAND(class, "set player class");
#endif
//FF_AUTO_COMMAND(ff_testcmd, TestCommand_f, "a test command", FCV
//ConCommand cc_ff_testcmd("ff_testcmd",

