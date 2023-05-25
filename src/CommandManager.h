/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_ENGINE_COMMAND_MANAGER_H
#define SOURCESHARP_ENGINE_COMMAND_MANAGER_H

#include "engine_shared.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

class edict_t;
class CCommand;
class ConCommand;

struct CommandInfo_t {
    ConCommand* m_pCommand;
    bool        m_bRegister;
    int         m_bHookId;

    CommandInfo_t()
    {
        m_pCommand  = nullptr;
        m_bRegister = false;
        m_bHookId   = 0;
    }
};

class CommandManager : public ISourceSharpModule
{
public:
    void OnLoad();
    void OnShutdown();

    void ConCommandDispatch(const CCommand& command);

private:
    ConCommand* FindOrCreateCommand(const char* name);
    int         GetCommandClient() const
    {
        return m_LastCommandClient;
    }

    void Hook_ClientCommand(edict_t* pClient, const CCommand& command);
    void Hook_SetCommandClient(int client);
    void Hook_CommandDispatch(const CCommand& command);

public:
    void RegisterServerCommand(const char* name);
    void RegisterClientCommand(const char* name);

private:
    std::unordered_set<std::string> m_ServerCommands;
    std::unordered_set<std::string> m_ClientCommands;

    std::unordered_map<std::string, CommandInfo_t*> m_Commands;

    int m_LastCommandClient;
    int m_HookId;
};

extern CommandManager g_CommandManager;

#endif // SOURCESHARP_ENGINE_COMMAND_MANAGER_H
