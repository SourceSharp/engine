/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_ENGINE_CONVAR_MANAGER_H
#define SOURCESHARP_ENGINE_CONVAR_MANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "engine_shared.h"
#include "modules/convar.h"

class ConVar;
class SSConVar;

class ConVarManager : public ISourceSharpModule
{
public:
    void OnLoad();
    void OnShutdown();

private:
    // SourceHook
    void Hook_ConVarChanged(ConVar* pConVar, const char* oldValue, float flOldValue);

public:
    SSConVar* CreateConVar(const char* pName, const char* pDefValue, const char* pDescription, int nFlags, bool bHasMin, float flMin, bool bHasMax, float flMax);
    SSConVar* FindConVar(const char* pName);
    bool      InstallGlobalChangeHook(SSConVar* pConVar);

private:
    int                                        m_HookId;
    std::unordered_set<ConVar*>                m_ChangedHooks;
    std::unordered_map<std::string, SSConVar*> m_ConVars;
};

extern ConVarManager g_ConVarManager;

#endif // SOURCESHARP_ENGINE_CONVAR_MANAGER_H