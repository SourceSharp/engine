/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#include "sourcesharp.h"
#include "SourceSharp.Runtime.h"
#include "engine.h"
#include <cstdio>

SourceSharp             g_SourceSharp;
IServerGameDLL*         server            = nullptr;
IServerGameClients*     gameclients       = nullptr;
IVEngineServer*         engine            = nullptr;
IServerPluginHelpers*   helpers           = nullptr;
IGameEventManager2*     gameevents        = nullptr;
IServerPluginCallbacks* vsp_callbacks     = nullptr;
IPlayerInfoManager*     playerinfomanager = nullptr;
ICvar*                  icvar             = nullptr;
CGlobalVars*            gpGlobals         = nullptr;

SH_DECL_HOOK6(IServerGameDLL, LevelInit, SH_NOATTRIB, 0, bool, char const*, char const*, char const*, char const*, bool,
              bool);

SH_DECL_HOOK3_void(IServerGameDLL, ServerActivate, SH_NOATTRIB, 0, edict_t*, int, int);

SH_DECL_HOOK0_void(IServerGameDLL, LevelShutdown, SH_NOATTRIB, 0);

SH_DECL_HOOK2_void(IServerGameClients, ClientActive, SH_NOATTRIB, 0, edict_t*, bool);

SH_DECL_HOOK1_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, 0, edict_t*);

SH_DECL_HOOK2_void(IServerGameClients, ClientPutInServer, SH_NOATTRIB, 0, edict_t*, char const*);

SH_DECL_HOOK1_void(IServerGameClients, ClientSettingsChanged, SH_NOATTRIB, 0, edict_t*);

SH_DECL_HOOK5(IServerGameClients, ClientConnect, SH_NOATTRIB, 0, bool, edict_t*, const char*, const char*, char*, int);

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent*, bool);

/**
 * Something like this is needed to register cvars/CON_COMMANDs.
 */
class BaseAccessor : public IConCommandBaseAccessor
{
public:
    bool RegisterConCommandBase(ConCommandBase* pCommandBase)
    {
        /* Always call META_REGCVAR instead of going through the engine. */
        return META_REGCVAR(pCommandBase);
    }
} s_BaseAccessor;

ICvar* g_pCvar;

PLUGIN_EXPOSE(SourceSharp, g_SourceSharp);

bool SourceSharp::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    m_bIsLoaded = false;

    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetEngineFactory, gameevents, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
    GET_V_IFACE_CURRENT(GetEngineFactory, helpers, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
    GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
    GET_V_IFACE_CURRENT(GetServerFactory, gameclients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
    GET_V_IFACE_CURRENT(GetServerFactory, playerinfomanager, IPlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER);
    gpGlobals = ismm->GetCGlobals();

    /* Load the VSP listener.  This is usually needed for IServerPluginHelpers. */
    if ((vsp_callbacks = ismm->GetVSPInfo(NULL)) == NULL)
    {
        ismm->AddListener(this, this);
        ismm->EnableVSPListener();
    }

    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, LevelInit, server, this, &SourceSharp::Hook_LevelInit, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, ServerActivate, server, this, &SourceSharp::Hook_ServerActivate, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, LevelShutdown, server, this, &SourceSharp::Hook_LevelShutdown, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientActive, gameclients, this, &SourceSharp::Hook_ClientActive, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, gameclients, this, &SourceSharp::Hook_ClientDisconnect, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, gameclients, this, &SourceSharp::Hook_ClientPutInServer, true);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientSettingsChanged, gameclients, this, &SourceSharp::Hook_ClientSettingsChanged, false);
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientConnect, gameclients, this, &SourceSharp::Hook_ClientConnect, false);

    g_pCVar = icvar;
    ConVar_Register(0, &s_BaseAccessor);

    g_Core.Load();

    return true;
}

bool SourceSharp::Unload(char* error, size_t maxlen)
{
    if (m_bIsLoaded)
    {
        m_bIsLoaded = false;
        ShutdownSourceSharp();
    }

    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, LevelInit, server, this, &SourceSharp::Hook_LevelInit, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, ServerActivate, server, this, &SourceSharp::Hook_ServerActivate, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, LevelShutdown, server, this, &SourceSharp::Hook_LevelShutdown, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientActive, gameclients, this, &SourceSharp::Hook_ClientActive, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientDisconnect, gameclients, this, &SourceSharp::Hook_ClientDisconnect, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, gameclients, this, &SourceSharp::Hook_ClientPutInServer, true);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientSettingsChanged, gameclients, this, &SourceSharp::Hook_ClientSettingsChanged, false);
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientConnect, gameclients, this, &SourceSharp::Hook_ClientConnect, false);

    g_Core.Unload();

    return true;
}

void SourceSharp::OnVSPListening(IServerPluginCallbacks* iface)
{
    vsp_callbacks = iface;
}

void SourceSharp::Hook_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
    META_LOG(g_PLAPI, "ServerActivate() called: edictCount = %d, clientMax = %d", edictCount, clientMax);
}

void SourceSharp::AllPluginsLoaded()
{
}

void SourceSharp::Hook_ClientActive(edict_t* pEntity, bool bLoadGame)
{
    META_LOG(g_PLAPI, "Hook_ClientActive(%d, %d)", IndexOfEdict(pEntity), bLoadGame);
}

void SourceSharp::Hook_ClientSettingsChanged(edict_t* pEdict)
{
    if (playerinfomanager)
    {
        IPlayerInfo* playerinfo = playerinfomanager->GetPlayerInfo(pEdict);

        const char* name = engine->GetClientConVarValue(IndexOfEdict(pEdict), "name");

        if (playerinfo != NULL
            && name != NULL
            && strcmp(engine->GetPlayerNetworkIDString(pEdict), "BOT") != 0
            && playerinfo->GetName() != NULL
            && strcmp(name, playerinfo->GetName()) == 0)
        {
            char msg[128];
            MM_Format(msg, sizeof(msg), "Your name changed to \"%s\" (from \"%s\")\n", name, playerinfo->GetName());
            engine->ClientPrintf(pEdict, msg);
        }
    }
}

bool SourceSharp::Hook_ClientConnect(edict_t*    pEntity,
                                     const char* pszName,
                                     const char* pszAddress,
                                     char*       reject,
                                     int         maxrejectlen)
{
    META_LOG(g_PLAPI, "Hook_ClientConnect(%d, \"%s\", \"%s\")", IndexOfEdict(pEntity), pszName, pszAddress);

    return true;
}

void SourceSharp::Hook_ClientPutInServer(edict_t* pEntity, char const* playername)
{
    // KeyValues *kv = new KeyValues( "msg" );
    // kv->SetString( "title", "Hello" );
    // kv->SetString( "msg", "Hello there" );
    // kv->SetColor( "color", Color( 255, 0, 0, 255 ));
    // kv->SetInt( "level", 5);
    // kv->SetInt( "time", 10);
    // helpers->CreateMessage(pEntity, DIALOG_MSG, kv, vsp_callbacks);
    // kv->deleteThis();
}

void SourceSharp::Hook_ClientDisconnect(edict_t* pEntity)
{
    META_LOG(g_PLAPI, "Hook_ClientDisconnect(%d)", IndexOfEdict(pEntity));
}

bool SourceSharp::Hook_LevelInit(const char* pMapName,
                                 const char* pMapEntities,
                                 const char* pOldLevel,
                                 const char* pLandmarkName,
                                 bool        loadGame,
                                 bool        background)
{
    if (m_bIsLoaded)
    {
        META_LOG(g_PLAPI, "Hook_LevelInit(%s)", pMapName);
    }
    else
    {
        const auto loadResult = InitializeSourceSharp();
        if (loadResult != 0)
        {
            Error("Failed to initialize SourceSharp, code: %d", loadResult);
        }
        m_bIsLoaded = true;
    }

    return true;
}

void SourceSharp::Hook_LevelShutdown()
{
    META_LOG(g_PLAPI, "Hook_LevelShutdown()");
}

bool SourceSharp::Pause(char* error, size_t maxlen)
{
    return true;
}

bool SourceSharp::Unpause(char* error, size_t maxlen)
{
    return true;
}

const char* SourceSharp::GetLicense()
{
    return "Apache-2.0 license";
}

const char* SourceSharp::GetVersion()
{
    return "1.0.0.0";
}

const char* SourceSharp::GetDate()
{
    return __DATE__;
}

const char* SourceSharp::GetLogTag()
{
    return "SourceSharp";
}

const char* SourceSharp::GetAuthor()
{
    return "SourceSharp Team";
}

const char* SourceSharp::GetDescription()
{
    return "SourceSharp";
}

const char* SourceSharp::GetName()
{
    return "SourceSharp";
}

const char* SourceSharp::GetURL()
{
    return "https://github.com/SourceSharp";
}
