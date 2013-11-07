#ifndef FF_SH_LUAUTIL_H
#define FF_SH_LUAUTIL_H
#pragma once

struct lua_State;

void LUAUTIL_RemoveKeysFromTable( lua_State *L, const char *pszTableName, const char** ppszKeys );
void LUAUTIL_RemoveVarsFromGlobal( lua_State *L, const char** ppszVars );
void LUAUTIL_StackDump( lua_State *L );

#endif