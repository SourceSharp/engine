/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_ENGINE_ENGINE_H
#define SOURCESHARP_ENGINE_ENGINE_H

#include "Core.h"
#include "sourcesharp.h"
#include "engine_interfaces.h"

#include <string>
#include <unordered_set>
#include <ctime>



struct edict_t;
class CCommand;

class Core
{
public:
    void Load();
    void Unload();

private:
    void Hook_GameFrame(bool simulating) const;

// SourceSharp
public:
    void LogError(const char* pMessage) const
    {
        META_LOG(m_pPlugin, "[%s] INFO: ", GetTimeString(time(nullptr)).c_str(), pMessage);
    }
    void LogMessage(const char* pMessage) const
    {
        META_LOG(m_pPlugin, "[%s] Fail: ", GetTimeString(time(nullptr)).c_str(), pMessage);
    }

    const char* GetGamePath() const {
        const char*  pBaseDir = m_pApi->GetBaseDir();
        const size_t len = strlen(pBaseDir);
        for (size_t i = len - 1; i < len; i--)
        {
            #if _WIN32
            if (pBaseDir[i] == '\\' || pBaseDir[i] == '/')
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
        return gpGlobals->maxClients;
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

private:
    static std::string GetTimeString(time_t time)
    {
        char buffer[64];
        auto timeinfo = localtime(&time);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

private:
    std::unordered_set<std::string> m_ServerCommands;
    std::unordered_set<std::string> m_ClientCommands;

private:
    ISmmAPI* m_pApi;
    ISmmPlugin* m_pPlugin;
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

#endif // SOURCESHARP_ENGINE_ENGINE_H
