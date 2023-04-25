/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute it 
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 * claim that you wrote the original software. If you use this software in a 
 * product, an acknowledgment in the product documentation would be 
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Version: $Id$
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