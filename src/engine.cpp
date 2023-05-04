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

#include "CommandManager.h"

Core g_Core;

SH_DECL_HOOK1_void(IServerGameDLL, GameFrame, SH_NOATTRIB, false, bool);


void Core::Load()
{
    m_pApi    = g_SMAPI;
    m_pPlugin = g_PLAPI;

    g_CommandManager.OnLoad();

    SH_ADD_HOOK(IServerGameDLL, GameFrame, server, SH_MEMBER(this, &Core::Hook_GameFrame), false);
}

void Core::Unload()
{
    g_CommandManager.OnShutdown();

    m_pApi = nullptr;
    m_pPlugin = nullptr;
}

void Core::Hook_GameFrame(bool simulating) const
{
    //OnGameFrame(simulating, gpGlobals->tickcount, gpGlobals->curtime);
}


//////////////////////////////////////////////////

CORE_INVOKE_FUNC_p1_void(LogError, const char*, pMessage);
CORE_INVOKE_FUNC_p1_void(LogMessage, const char*, pMessage);
CORE_INVOKE_FUNC_p0(GetGamePath, const char*);
CORE_INVOKE_FUNC_p0(GetMaxClients, int);
CORE_INVOKE_FUNC_p0(GetMaxHumanPlayers, int);
CORE_INVOKE_FUNC_p0(GetEngineVersion, int);