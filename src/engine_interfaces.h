/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef _INCLUDE_SOURCE_ENGINE_INTERFACES_
#define _INCLUDE_SOURCE_ENGINE_INTERFACES_

#include <eiface.h>
#include <igameevents.h>
#include <iplayerinfo.h>

extern IServerGameDLL*         server;
extern IServerGameClients*     gameclients;
extern IVEngineServer*         engine;
extern IServerPluginHelpers*   helpers;
extern IGameEventManager2*     gameevents;
extern IServerPluginCallbacks* vsp_callbacks;
extern IPlayerInfoManager*     playerinfomanager;
extern ICvar*                  icvar;
extern CGlobalVars*            gpGlobals;

#endif // _INCLUDE_SOURCE_ENGINE_INTERFACES_