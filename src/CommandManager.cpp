#include "CommandManager.h"

#include <SourceSharp.Runtime.h>
#include <convar.h>
#include <edict.h>
#include <engine_interfaces.h>
#include <sourcehook.h>

#include "sourcesharp.h"
#include "modules/concommand.h"

CommandManager g_CommandManager;

SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t*, const CCommand&);
SH_DECL_HOOK1_void(IServerGameClients, SetCommandClient, SH_NOATTRIB, false, int);
SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, false, const CCommand&);

void OnCommandDispatch(const CCommand& command)
{
    g_CommandManager.ConCommandDispatch(command);
}

void CommandManager::OnLoad()
{
    m_LastCommandClient = -1;
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &CommandManager::Hook_ClientCommand, false);
    m_HookId = SH_ADD_HOOK(IServerGameClients, SetCommandClient, gameclients, SH_MEMBER(this, &CommandManager::Hook_SetCommandClient), false);

    // TODO Say Hook
    // "say", "say_steam", "say_squad"
}

void CommandManager::OnShutdown()
{
    SH_REMOVE_HOOK_ID(m_HookId);

    for (const auto& [key, info] : m_Commands)
    {
         if (info->m_bRegister)
            g_SMAPI->UnregisterConCommandBase(g_PLAPI, info->m_pCommand);
        SH_REMOVE_HOOK_ID(info->m_bHookId);
        delete info;
    }
    m_Commands.clear();
    m_ClientCommands.clear();
    m_ServerCommands.clear();

    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &CommandManager::Hook_ClientCommand, false);
}

ConCommand* CommandManager::FindOrCreateCommand(const char* name)
{
    const auto key  = std::string(name);
    const auto info = m_Commands.find(key);
    if (info != m_Commands.end())
        return info->second->m_pCommand;

    auto* pInfo = new CommandInfo_t;
    auto* pCommand = g_pCVar->FindCommand(name);
    if (!pCommand)
    {
        pInfo->m_pCommand  = new ConCommand(V_strdup(name), OnCommandDispatch, "No Help String", FCVAR_GAMEDLL);
        pInfo->m_bRegister = true;
    }
    else
    {
        pInfo->m_pCommand = pCommand;
    }

    if (!pInfo->m_bHookId && !pInfo->m_bRegister)
        pInfo->m_bHookId = SH_ADD_HOOK(ConCommand, Dispatch, pCommand, SH_MEMBER(this, &CommandManager::Hook_CommandDispatch), false);

    m_Commands[key] = pInfo;

    return pCommand;
}

void CommandManager::Hook_SetCommandClient(int client)
{
    m_LastCommandClient = client + 1;
}

void CommandManager::Hook_ClientCommand(edict_t* pClient, const CCommand& command)
{
    // TODO 
    // OnClientCommand in IPlayerManager

    if (!pClient || pClient->IsFree())
        return;

    Msg("Hook_ClientCommand(%d, [%s]);\n", IndexOfEdict(pClient), command.ArgS());

    RETURN_META(MRES_SUPERCEDE);
}

void CommandManager::RegisterClientCommand(const char* name)
{
    m_ClientCommands.emplace(name);
    FindOrCreateCommand(name);
}

void CommandManager::RegisterServerCommand(const char* name)
{
    m_ServerCommands.emplace(name);
    FindOrCreateCommand(name);
}

void CommandManager::Hook_CommandDispatch(const CCommand& command)
{
    const auto client = GetCommandClient();
    const auto pName  = command.Arg(0);

    auto maxRet = 0;
    for (auto& name : m_ServerCommands)
    {
        if (V_strcasecmp(name.c_str(), pName) == 0)
        {
            const auto ret = ServerConsoleCommand(command.ArgS(), command.ArgV(), command.ArgC());
            if (ret > maxRet)
            {
                maxRet = ret;
            }
        }
    }

    for (auto& name : m_ClientCommands)
    {
        if (V_strcasecmp(name.c_str(), pName) == 0)
        {
            const auto ret = ClientConsoleCommand(command.ArgS(), command.ArgV(), command.ArgC(), client);
            if (ret > maxRet)
            {
                maxRet = ret;
            }
        }
    }

    if (maxRet)
    {
        RETURN_META(MRES_SUPERCEDE);
    }
}

void CommandManager::ConCommandDispatch(const CCommand& command)
{
    const auto client = GetCommandClient();
    const auto pName   = command.Arg(0);

    auto maxRet = 0;
    for (auto& name : m_ServerCommands)
    {
        if (V_strcasecmp(name.c_str(), pName) == 0)
        {
            const auto ret = ServerConsoleCommand(command.ArgS(), command.ArgV(), command.ArgC());
            if (ret > maxRet)
            {
                maxRet = ret;
            }
        }
    }

    for (auto& name : m_ClientCommands)
    {
        if (V_strcasecmp(name.c_str(), pName) == 0)
        {
            const auto ret = ClientConsoleCommand(command.ArgS(), command.ArgV(), command.ArgC(), client);
            if (ret > maxRet)
            {
                maxRet = ret;
            }
        }
    }
}


SS_API void RegClientCommand(const char* name)
{
    g_CommandManager.RegisterClientCommand(name);
}

SS_API void RegServerCommand(const char* name)
{
    g_CommandManager.RegisterServerCommand(name);
}