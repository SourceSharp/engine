/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#include "engine.h"
#include <SourceSharp.Runtime.h>
#include <sourcesharp.h>
#include <vector>

Core g_Core;

#if SOURCE_ENGINE >= SE_ORANGEBOX

SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t*,
                   const CCommand&);
#else
SH_DECL_HOOK1_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0,
                   edict_t*);
#endif

extern IServerGameClients* gameclients;

void Core::Load()
{
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this,
                        &Core::Hook_ClientCommand, false);
}

void Core::Unload()
{
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this,
                           &Core::Hook_ClientCommand, false);
}

#if SOURCE_ENGINE >= SE_ORANGEBOX

void Core::Hook_ClientCommand(edict_t* pEntity, const CCommand& args)
#else
void Core::Hook_ClientCommand(edict_t* pEntity)
#endif
{
#if SOURCE_ENGINE <= SE_DARKMESSIAH
    CCommand args;
#endif

    if (!pEntity || pEntity->IsFree())
    {
        return;
    }

    const char* cmd = args.Arg(0);
    if (m_ClientCommands.find(cmd) != m_ClientCommands.end()) // not test
    {
        ClientConsoleCommand(args.ArgS(), args.ArgV(), args.ArgC(), 3);
        RETURN_META(MRES_SUPERCEDE);
    }
}

void Core::RegServerCommand(const char* command)
{
    // TODO new ConCommand
    m_ServerCommands.emplace(command);
}

void Core::RegClientCommand(const char* command)
{
    // TODO new ConCommand
    m_ClientCommands.emplace(command);
}

////////////////////////////////////////////////////////////////////

CORE_INVOKE_FUNC_p1_void(LogError, const char*, pMessage);
CORE_INVOKE_FUNC_p1_void(LogMessage, const char*, pMessage);
CORE_INVOKE_FUNC_p0(GetGamePath, const char*);
CORE_INVOKE_FUNC_p0(GetMaxClients, int);
CORE_INVOKE_FUNC_p0(GetMaxHumanPlayers, int);
CORE_INVOKE_FUNC_p0(GetEngineVersion, int);

CORE_INVOKE_FUNC_p1_void(RegServerCommand, const char*, command);
CORE_INVOKE_FUNC_p1_void(RegClientCommand, const char*, command);

// ICore *GetInterfaceCore() {
//     return (ICore * ) & g_Core;
// }