/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SAMPLE_MM_CORE_H
#define SAMPLE_MM_CORE_H

#include "Core.h"
#include "sourcesharp.h"

#include <string>
#include <unordered_set>
#include <ctime>

#include <igameevents.h>
#include <iplayerinfo.h>
#include <iconvar.h>

struct edict_t;
class CCommand;

extern IServerGameDLL*         server;
extern IServerGameClients*     gameclients;
extern IVEngineServer*         engine;
extern IServerPluginHelpers*   helpers;
extern IGameEventManager2*     gameevents;
extern IServerPluginCallbacks* vsp_callbacks;
extern IPlayerInfoManager*     playerinfomanager;
extern ICvar*                  icvar;
extern CGlobalVars*            gpGlobals;


class Core
{
public:
    void Load();

    void Unload();

// SourceSharp
public:
    void LogError(const char* pMessage) const
    {
        META_LOG(reinterpret_cast<ISmmPlugin*>(&g_SourceSharp), "[%s] INFO: ", GetTimeString(time(nullptr)), pMessage);
    }
    void LogMessage(const char* pMessage)
    {
        META_LOG(reinterpret_cast<ISmmPlugin*>(&g_SourceSharp), "[%s] Fail: ", GetTimeString(time(nullptr)), pMessage);
    }

    const char* GetGamePath() const {
        const char* pBaseDir = g_SMAPI->GetBaseDir();
        const size_t len = strlen(pBaseDir);
        char path[256];
        for (size_t i = len - 1; i < len; i--)
        {
            #if _WIN32
            if (pBaseDir[i] == '\\')
            #else
            if (pBaseDir[i] == '/')
            #endif
            {
                return &pBaseDir[++i];
            }
        }
        Error("Failed to GetGamePath!");
        return pBaseDir;
    }

    int GetMaxClients() const {
        // TODO 从engine读取
        #if SOURCE_ENGINE == ENGINE_CSGO
        return 64;
        #else
        int min, max, def;
        gameclients->GetPlayerLimits(min, max, def);
        return max;
        #endif
    }
    int GetMaxHumanPlayers() const {return gameclients->GetMaxHumanPlayers();}
    int GetEngineVersion() const { return SOURCE_ENGINE; }
    void ExecuteServerCommand(const char* pCommand) const { engine->ServerCommand(pCommand); }
    void InsertServerCommand(const char* pCommand) const { engine->InsertServerCommand(pCommand); }
    void ServerExecute() const { engine->ServerExecute(); }

// ConCommand
public:
    void RegServerCommand(const char* command);
    void RegClientCommand(const char* command);

private:
#if SOURCE_ENGINE >= SE_ORANGEBOX
    void Hook_ClientCommand(edict_t* pEntity, const CCommand& args);
#else
    void Hook_ClientCommand(edict_t* pEntity);
#endif

private:
    const char* GetTimeString(time_t time) const
    {
        char buffer[64];
        auto timeinfo = localtime(&time);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return buffer;
    }

private:
    std::unordered_set<std::string> m_ServerCommands;
    std::unordered_set<std::string> m_ClientCommands;
};

extern Core g_Core;

#define CORE_INVOKE_FUNC_p0_void(FUNCNAME) \
    void FUNCNAME() \
    { \
        g_Core.FUNCNAME(); \
    }

#define CORE_INVOKE_FUNC_p0(FUNCNAME, RETRUNV) \
    RETRUNV FUNCNAME() \
    { \
        return g_Core.FUNCNAME(); \
    }

#define CORE_INVOKE_FUNC_p1(FUNCNAME, RETRUNV, t1, a1) \
    RETRUNV FUNCNAME(t1 a1) \
    { \
        return g_Core.FUNCNAME(a1); \
    }

#define CORE_INVOKE_FUNC_p1_void(FUNCNAME, t1, a1) \
    void FUNCNAME(t1 a1) \
    { \
        g_Core.FUNCNAME(a1); \
    }

#endif // SAMPLE_MM_CORE_H
