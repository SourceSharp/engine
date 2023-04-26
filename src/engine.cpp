/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#include <vector>
#include <sourcesharp.h>
#include <SourceSharp.Runtime.h>

#include "engine.h"


#if SOURCE_ENGINE >= SE_ORANGEBOX

SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t*, const CCommand&);
#else
SH_DECL_HOOK1_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t*);
#endif

extern IServerGameClients *gameclients;


void Core::Load() {
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &Core::Hook_ClientCommand, false);
}

void Core::Unload() {
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &Core::Hook_ClientCommand, false);
}

#if SOURCE_ENGINE >= SE_ORANGEBOX

void Core::Hook_ClientCommand(edict_t *pEntity, const CCommand &args)
#else
void Core::Hook_ClientCommand(edict_t* pEntity)
#endif
{
#if SOURCE_ENGINE <= SE_DARKMESSIAH
    CCommand args;
#endif

    if (!pEntity || pEntity->IsFree()) {
        return;
    }

    const char *cmd = args.Arg(0);
    if (m_ClientCommands.contains(cmd)) {
        ClientConsoleCommand(args.ArgS(), args.ArgV(), args.ArgC(), 3);
        RETURN_META(MRES_SUPERCEDE);
    }
}

void Core::RegServerCommand(const char *command) {
    m_ServerCommands.emplace(command);
}

void Core::RegClientCommand(const char *command) {
    m_ClientCommands.emplace(command);
}

Core g_Core;

void RegServerCommand(const char *command) {
    g_Core.RegServerCommand(command);
}

void RegClientCommand(const char *command) {
    g_Core.RegClientCommand(command);
}

//ICore *GetInterfaceCore() {
//    return (ICore * ) & g_Core;
//}