/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include "engine_wrappers.h"
#include <ISmmPlugin.h>

struct edict_t;
class CCommand;

#if defined WIN32 && !defined snprintf
#    define snprintf _snprintf
#endif

class SourceSharp : public ISmmPlugin,
                    public IMetamodListener
{
public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen,
              bool late);
    bool Unload(char* error, size_t maxlen);
    bool Pause(char* error, size_t maxlen);
    bool Unpause(char* error, size_t maxlen);
    void AllPluginsLoaded();

public: // IMetamodListener stuff
    void OnVSPListening(IServerPluginCallbacks* iface);

public: // hooks
    void Hook_ServerActivate(edict_t* pEdictList, int edictCount,
                             int clientMax);
    bool Hook_LevelInit(const char* pMapName, char const* pMapEntities,
                        char const* pOldLevel, char const* pLandmarkName,
                        bool loadGame, bool background);
    void Hook_GameFrame(bool simulating);
    void Hook_LevelShutdown(void);
    void Hook_ClientActive(edict_t* pEntity, bool bLoadGame);
    void Hook_ClientDisconnect(edict_t* pEntity);
    void Hook_ClientPutInServer(edict_t* pEntity, char const* playername);
    void Hook_SetCommandClient(int index);
    void Hook_ClientSettingsChanged(edict_t* pEdict);
    bool Hook_ClientConnect(edict_t* pEntity, const char* pszName,
                            const char* pszAddress, char* reject,
                            int maxrejectlen);
#if SOURCE_ENGINE >= SE_ORANGEBOX
    void Hook_ClientCommand(edict_t* pEntity, const CCommand& args);
#else
    void Hook_ClientCommand(edict_t* pEntity);
#endif
public:
    const char* GetAuthor();
    const char* GetName();
    const char* GetDescription();
    const char* GetURL();
    const char* GetLicense();
    const char* GetVersion();
    const char* GetDate();
    const char* GetLogTag();
};

// TODO
// https://github.com/alliedmodders/sourcemod/blob/97f2fc9d7fa49d334c6b70bb8b0d779b11a74ee5/core/sourcemm_api.cpp#L46

extern SourceSharp g_SourceSharp;

PLUGIN_GLOBALVARS();

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
