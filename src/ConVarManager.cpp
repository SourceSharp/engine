#include "ConVarManager.h"

#include <SourceSharp.Runtime.h>
#include <convar.h>
#include <edict.h>
#include <engine_interfaces.h>
#include <sourcehook.h>

#include "modules/convar.h"
#include "sourcesharp.h"

ConVarManager g_ConVarManager;

SH_DECL_HOOK3_void(ICvar, CallGlobalChangeCallbacks, SH_NOATTRIB, false, ConVar*, const char*, float);

void ConVarManager::OnLoad()
{
    m_HookId = SH_ADD_HOOK(ICvar, CallGlobalChangeCallbacks, icvar, SH_MEMBER(this, &ConVarManager::Hook_ConVarChanged), false);
}

void ConVarManager::OnShutdown()
{
    SH_REMOVE_HOOK_ID(m_HookId);
}

void ConVarManager::Hook_ConVarChanged(ConVar* pConVar, const char* oldValue, float flOldValue)
{
    const auto name = pConVar->GetName();
    if (m_ChangedHooks.find(pConVar) == m_ChangedHooks.end())
        return;

    const auto pVar = FindConVar(name);
    if (pVar == nullptr)
        return;

    OnConVarChanged(name, oldValue, pConVar->GetString());
}

bool ConVarManager::InstallGlobalChangeHook(SSConVar* pConVar)
{
    const auto pCvar = pConVar->GetBase();
    if (m_ChangedHooks.find(pCvar) != m_ChangedHooks.end())
        return false;

    m_ChangedHooks.insert(pCvar);
    return true;
}

SSConVar* ConVarManager::CreateConVar(const char* pName, const char* pDefValue, const char* pDescription, int nFlags, bool bHasMin, float flMin, bool bHasMax, float flMax)
{
    auto pVar = FindConVar(pName);
    if (pVar != nullptr)
        return pVar;

    if (g_pCVar->FindCommand(pName))
        return nullptr;

    const auto name = strdup(pName);
    const auto defV = strdup(pDefValue);
    const auto desc = strdup(pDescription);
    V_strlower(name);

    const auto pCvar             = new ConVar(name, defV, nFlags, desc, bHasMin, flMin, bHasMax, flMax);
    pVar                         = new SSConVar(pCvar, false);
    m_ConVars[std::string(name)] = pVar;

    return pVar;
}

SSConVar* ConVarManager::FindConVar(const char* pName)
{
    const auto key = std::string(pName);
    const auto var = m_ConVars.find(key);
    if (var != m_ConVars.end())
    {
        // return already cached convar
        return var->second;
    }

    // find engine convar
    const auto pCvar = g_pCVar->FindVar(pName);
    if (pCvar == nullptr)
    {
        return nullptr;
    }

    const auto name          = pCvar->GetName();
    const auto pVar          = new SSConVar(pCvar, false);
    m_ConVars[std::string()] = pVar;
    return pVar;
}

inline SSConVar::SSConVar(ConVar* pVar, bool bRegister) :
    m_pConVar(pVar), m_bRegister(bRegister)
{
}

#define CVAR_BRIDGE_GET(member, ret)                    \
    SS_API ret SSConVarGet##member(const SSConVar* ptr) \
    {                                                   \
        return ptr->GetBase()->Get##member();           \
    }

#define CVAR_BRIDGE_SET(member, type)                              \
    SS_API void SSConVarSet##member(const SSConVar* ptr, type val) \
    {                                                              \
        return ptr->GetBase()->SetValue(val);                      \
    }

CVAR_BRIDGE_GET(Name, const char*);
CVAR_BRIDGE_GET(Default, const char*);

DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, Description, const char*)
{
    return ptr->GetBase()->GetHelpText();
}

// Value
CVAR_BRIDGE_GET(String, const char*);
CVAR_BRIDGE_GET(Float, float);
CVAR_BRIDGE_GET(Int, int);
CVAR_BRIDGE_GET(Bool, bool);
CVAR_BRIDGE_SET(String, const char*);
CVAR_BRIDGE_SET(Float, float);
CVAR_BRIDGE_SET(Int, int);
CVAR_BRIDGE_SET(Bool, bool);

// Flags
CVAR_BRIDGE_GET(Flags, int);
SS_API void SSConVarAddFlags(const SSConVar* pVar, int nFlags)
{
    pVar->GetBase()->AddFlags(nFlags);
}

// Bounds
CVAR_BRIDGE_GET(MinValue, float);
CVAR_BRIDGE_GET(MaxValue, float);

DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, HasMin, bool)
{
    return ptr->GetBase()->HasMin();
}

DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, HasMax, bool)
{
    return ptr->GetBase()->HasMax();
}

SS_API void SSConVarSetBound(const SSConVar* pVar,
                             bool            bHasMin,
                             float           flMin,
                             bool            bHasMax,
                             float           flMax)
{
    // todo
    pVar->GetBase()->SetValue(1);
};

// Function
SS_API void SSConVarRevert(const SSConVar* pVar)
{
    return pVar->GetBase()->Revert();
}

SS_API bool SSConVarReplicateToPlayers(const SSConVar* pVar, const int pPlayers[], int nPlayers)
{
    return pVar->ReplicateToPlayers(pPlayers, nPlayers);
}

// Global
SS_API SSConVar* CreateConVar(const char* pName,
                              const char* pDefValue,
                              const char* pDescription,
                              const int   nFlags,
                              const bool  bHasMin,
                              const float flMin,
                              const bool  bHasMax,
                              const float flMax)
{
    return g_ConVarManager.CreateConVar(pName, pDefValue, pDescription, nFlags, bHasMin, flMin, bHasMax, flMax);
}

SS_API SSConVar* FindConVar(const char* pName)
{
    return g_ConVarManager.FindConVar(pName);
}

SS_API bool RegisterConVarHook(const char* pName)
{
    auto* pVar = g_ConVarManager.FindConVar(pName);
    if (pVar == nullptr)
    {
        return false;
    }
    return g_ConVarManager.InstallGlobalChangeHook(pVar);
}

// Impl
bool SSConVar::ReplicateToPlayers(const int pPlayers[], int nPlayers) const
{
    // TODO

    const auto pName = GetBase()->GetName();

    return false;
}