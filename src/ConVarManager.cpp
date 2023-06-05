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
    if (m_ChangedHooks.find(pConVar) == m_ChangedHooks.end())
        return;

    const auto name = pConVar->GetName();
    const auto pVar = FindConVar(name);
    if (pVar == nullptr)
        return;

    OnConVarChanged(name, oldValue, pVar->GetString());
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

    auto name = strdup(pName);
    V_strlower(name);

    const auto pCvar = new ConVar(name, pDefValue, nFlags, pDescription, bHasMin, flMin, bHasMax, flMax);
    pVar = new SSConVar(pCvar, false);
    m_ConVars[std::string(pVar->GetName())] = pVar;

    return pVar;
}

SSConVar* ConVarManager::FindConVar(const char* pName)
{
    const auto key  = std::string(pName);
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

    const auto pVar = new SSConVar(pCvar, false);
    m_ConVars[std::string(pVar->GetName())] = pVar;
    return pVar;
}

inline SSConVar::SSConVar(ConVar* pVar, bool bRegister) :
    m_pConVar(pVar), m_bRegister(bRegister)
{
}

inline ConVar* SSConVar::GetBase()
{
    return m_pConVar;
}

#define SSCVAR_INVOKE_GET(NAME, TYPE)           \
    TYPE SSConVar::Get##NAME()     \
    {                                           \
        return m_pConVar->Get##NAME();          \
    }

#define SSCVAR_INVOKE_SET(NAME, TYPE)               \
    void SSConVar::Set##NAME(TYPE v)   \
    {                                               \
        m_pConVar->Set##NAME(v);                    \
    }

#define SSCVAR_INVOKE_SETV(NAME, TYPE)             \
    void SSConVar::Set##NAME(TYPE v) \
    {                                             \
        m_pConVar->SetValue(v);                  \
    }

// Info
SSCVAR_INVOKE_GET(Name, const char*);
SSCVAR_INVOKE_GET(Default, const char*);

const char* SSConVar::GetDescription()
{
    return m_pConVar->GetHelpText();
}

// Value
SSCVAR_INVOKE_GET(String, const char*);
SSCVAR_INVOKE_GET(Float, float);
SSCVAR_INVOKE_GET(Int, int);
SSCVAR_INVOKE_GET(Bool, bool);
SSCVAR_INVOKE_SETV(String, const char*);
SSCVAR_INVOKE_SETV(Float, float);
SSCVAR_INVOKE_SETV(Int, int);
SSCVAR_INVOKE_SETV(Bool, bool);

// Flags
SSCVAR_INVOKE_GET(Flags, int);

void SSConVar::SetFlags(int v)
{
    m_pConVar->AddFlags(v);
}


// Bounds
float SSConVar::GetMin()
{
    return m_pConVar->GetMinValue();
}

float SSConVar::GetMax()
{
    return m_pConVar->GetMinValue();
}

bool SSConVar::GetHasMin()
{
    float v;
    return m_pConVar->GetMin(v);
}

bool SSConVar::GetHasMax()
{
    float v;
    return m_pConVar->GetMax(v);
}

void SSConVar::SetMax(float value)
{
    // TODO
}

void SSConVar::SetHasMax(bool has)
{
    // TODO
}

void SSConVar::SetMin(float value)
{
    // TODO
}

void SSConVar::SetHasMin(bool has)
{
    // TODO
}

// User Message
bool SSConVar::ReplicateToPlayers(const int pPlayers[], int nPlayers)
{
    // TODO
    return false;
}

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