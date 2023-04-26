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

#include <stdio.h>
#include "sourcesharp.h"
#include "SourceSharp.Runtime.h"

#include "engine.h"

SH_DECL_HOOK6(IServerGameDLL, LevelInit, SH_NOATTRIB, 0, bool, char const*, char const*, char const*, char const*, bool,
              bool);

SH_DECL_HOOK3_void(IServerGameDLL, ServerActivate, SH_NOATTRIB, 0, edict_t*, int, int);

SH_DECL_HOOK1_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool);

SH_DECL_HOOK0_void(IServerGameDLL, LevelShutdown, SH_NOATTRIB, 0);

SH_DECL_HOOK2_void(IServerGameClients, ClientActive, SH_NOATTRIB, 0, edict_t*, bool);

SH_DECL_HOOK1_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, 0, edict_t*);

SH_DECL_HOOK2_void(IServerGameClients, ClientPutInServer, SH_NOATTRIB, 0, edict_t*, char const*);

SH_DECL_HOOK1_void(IServerGameClients, SetCommandClient, SH_NOATTRIB, 0, int);

SH_DECL_HOOK1_void(IServerGameClients, ClientSettingsChanged, SH_NOATTRIB, 0, edict_t*);

SH_DECL_HOOK5(IServerGameClients, ClientConnect, SH_NOATTRIB, 0, bool, edict_t*, const char*, const char*, char*, int);

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent*, bool);

SourceSharp g_SourceSharp;
IServerGameDLL *server = NULL;
IServerGameClients *gameclients = NULL;
IVEngineServer *engine = NULL;
IServerPluginHelpers *helpers = NULL;
IGameEventManager2 *gameevents = NULL;
IServerPluginCallbacks *vsp_callbacks = NULL;
IPlayerInfoManager *playerinfomanager = NULL;
ICvar *icvar = NULL;
CGlobalVars *gpGlobals = NULL;

/**
 * Something like this is needed to register cvars/CON_COMMANDs.
 */
class BaseAccessor : public IConCommandBaseAccessor {
public:
    bool RegisterConCommandBase(ConCommandBase *pCommandBase) {
        /* Always call META_REGCVAR instead of going through the engine. */
        return META_REGCVAR(pCommandBase);
    }
} s_BaseAccessor;

ICvar *g_pCvar;

PLUGIN_EXPOSE(SourceSharp, g_SourceSharp);

bool SourceSharp::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    PLUGIN_SAVEVARS();

    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetEngineFactory, gameevents, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
    GET_V_IFACE_CURRENT(GetEngineFactory, helpers, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
    GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
    GET_V_IFACE_ANY(GetServerFactory, gameclients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
    GET_V_IFACE_ANY(GetServerFactory, playerinfomanager, IPlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER);

    gpGlobals = ismm->GetCGlobals();

    /* Load the VSP listener.  This is usually needed for IServerPluginHelpers. */
    if ((vsp_callbacks = ismm->GetVSPInfo(NULL)) == NULL) {
        ismm->AddListener(this, this);
        ismm->EnableVSPListener();
    }

    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, LevelInit, server, this, &SourceSharp::Hook_LevelInit, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, ServerActivate, server, this, &SourceSharp::Hook_ServerActivate, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, this, &SourceSharp::Hook_GameFrame, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, LevelShutdown, server, this, &SourceSharp::Hook_LevelShutdown, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientActive, gameclients, this, &SourceSharp::Hook_ClientActive, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, gameclients, this, &SourceSharp::Hook_ClientDisconnect,
                        true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, gameclients, this, &SourceSharp::Hook_ClientPutInServer,
                        true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, SetCommandClient, gameclients, this, &SourceSharp::Hook_SetCommandClient,
                        true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientSettingsChanged, gameclients, this,
                        &SourceSharp::Hook_ClientSettingsChanged, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientConnect, gameclients, this, &SourceSharp::Hook_ClientConnect, false);

#if SOURCE_ENGINE >= SE_ORANGEBOX
    g_pCVar = icvar;
    ConVar_Register(0, &s_BaseAccessor);
#else
    ConCommandBaseMgr::OneTimeInit(&s_BaseAccessor);
#endif

    g_Core.Load();

    InitializeSourceSharp();

    return true;
}

bool SourceSharp::Unload(char *error, size_t maxlen) {
    ShutdownSourceSharp();

    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, LevelInit, server, this, &SourceSharp::Hook_LevelInit, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, ServerActivate, server, this, &SourceSharp::Hook_ServerActivate, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, this, &SourceSharp::Hook_GameFrame, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, LevelShutdown, server, this, &SourceSharp::Hook_LevelShutdown, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientActive, gameclients, this, &SourceSharp::Hook_ClientActive, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, gameclients, this,
                           &SourceSharp::Hook_ClientDisconnect, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, gameclients, this,
                           &SourceSharp::Hook_ClientPutInServer, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, SetCommandClient, gameclients, this,
                           &SourceSharp::Hook_SetCommandClient, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientSettingsChanged, gameclients, this,
                           &SourceSharp::Hook_ClientSettingsChanged, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientConnect, gameclients, this, &SourceSharp::Hook_ClientConnect,
                           false);

    g_Core.Unload();

    return true;
}

void SourceSharp::OnVSPListening(IServerPluginCallbacks *iface) {
    vsp_callbacks = iface;
}

void SourceSharp::Hook_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax) {
    META_LOG(g_PLAPI, "ServerActivate() called: edictCount = %d, clientMax = %d", edictCount, clientMax);
}

void SourceSharp::AllPluginsLoaded() {
    /* This is where we'd do stuff that relies on the mod or other plugins
     * being initialized (for example, cvars added and events registered).
     */
}

void SourceSharp::Hook_ClientActive(edict_t *pEntity, bool bLoadGame) {
    META_LOG(g_PLAPI, "Hook_ClientActive(%d, %d)", IndexOfEdict(pEntity), bLoadGame);
}


void SourceSharp::Hook_ClientSettingsChanged(edict_t *pEdict) {
    if (playerinfomanager) {
        IPlayerInfo *playerinfo = playerinfomanager->GetPlayerInfo(pEdict);

        const char *name = engine->GetClientConVarValue(IndexOfEdict(pEdict), "name");

        if (playerinfo != NULL
            && name != NULL
            && strcmp(engine->GetPlayerNetworkIDString(pEdict), "BOT") != 0
            && playerinfo->GetName() != NULL
            && strcmp(name, playerinfo->GetName()) == 0) {
            char msg[128];
            MM_Format(msg, sizeof(msg), "Your name changed to \"%s\" (from \"%s\")\n", name, playerinfo->GetName());
            engine->ClientPrintf(pEdict, msg);
        }
    }
}

bool SourceSharp::Hook_ClientConnect(edict_t *pEntity,
                                     const char *pszName,
                                     const char *pszAddress,
                                     char *reject,
                                     int maxrejectlen) {
    META_LOG(g_PLAPI, "Hook_ClientConnect(%d, \"%s\", \"%s\")", IndexOfEdict(pEntity), pszName, pszAddress);

    return true;
}

void SourceSharp::Hook_ClientPutInServer(edict_t *pEntity, char const *playername) {
    //KeyValues *kv = new KeyValues( "msg" );
    //kv->SetString( "title", "Hello" );
    //kv->SetString( "msg", "Hello there" );
    //kv->SetColor( "color", Color( 255, 0, 0, 255 ));
    //kv->SetInt( "level", 5);
    //kv->SetInt( "time", 10);
    //helpers->CreateMessage(pEntity, DIALOG_MSG, kv, vsp_callbacks);
    //kv->deleteThis();
}

void SourceSharp::Hook_ClientDisconnect(edict_t *pEntity) {
    META_LOG(g_PLAPI, "Hook_ClientDisconnect(%d)", IndexOfEdict(pEntity));
}

void SourceSharp::Hook_GameFrame(bool simulating) {
    /**
     * simulating:
     * ***********
     * true  | game is ticking
     * false | game is not ticking
     */
}

bool SourceSharp::Hook_LevelInit(const char *pMapName,
                                 char const *pMapEntities,
                                 char const *pOldLevel,
                                 char const *pLandmarkName,
                                 bool loadGame,
                                 bool background) {
    META_LOG(g_PLAPI, "Hook_LevelInit(%s)", pMapName);

    return true;
}

void SourceSharp::Hook_LevelShutdown() {
    META_LOG(g_PLAPI, "Hook_LevelShutdown()");
}

void SourceSharp::Hook_SetCommandClient(int index) {
    META_LOG(g_PLAPI, "Hook_SetCommandClient(%d)", index);
}

bool SourceSharp::Pause(char *error, size_t maxlen) {
    return true;
}

bool SourceSharp::Unpause(char *error, size_t maxlen) {
    return true;
}

const char *SourceSharp::GetLicense() {
    return "Public Domain";
}

const char *SourceSharp::GetVersion() {
    return "1.0.0.0";
}

const char *SourceSharp::GetDate() {
    return __DATE__;
}

const char *SourceSharp::GetLogTag() {
    return "SourceSharp";
}

const char *SourceSharp::GetAuthor() {
    return "SourceSharp Team";
}

const char *SourceSharp::GetDescription() {
    return "SourceSharp";
}

const char *SourceSharp::GetName() {
    return "SourceSharp";
}

const char *SourceSharp::GetURL() {
    return "https://github.com/SourceSharp";
}
