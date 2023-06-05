/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_CONVAR_H
#define SOURCESHARP_CONVAR_H

#include "_export.h"

class ConVar;

class SS_CLASS_EXPORT SSConVar
{
public:
    const char* GetName();
    const char* GetDefault();
    const char* GetDescription();

    const char* GetString();
    float       GetFloat();
    int         GetInt();
    bool        GetBool();

    void SetString(const char* pValue);
    void SetFloat(float value);
    void SetInt(int value);
    void SetBool(bool value);

    void SetFlags(int flags);
    int  GetFlags();

    void  SetMin(float value);
    float GetMin();
    bool  GetHasMin();
    void  SetHasMin(bool has);

    void  SetMax(float value);
    float GetMax();
    bool  GetHasMax();
    void  SetHasMax(bool has);

    bool ReplicateToPlayers(const int pPlayers[], int nPlayers);

    ConVar* GetBase();

private:
    ConVar* m_pConVar;
    bool    m_bRegister;

public:
    SSConVar(ConVar* pVar, bool bRegister);
};

SS_API const char* GetSSConVarName(SSConVar* pVar);
SS_API const char* GetSSConVarDefault(SSConVar* pVar);

SS_API SSConVar* CreateConVar(const char* pName,
                              const char* pDefValue,
                              const char* pDescription,
                              const int   nFlags,
                              const bool  bHasMin,
                              const float flMin,
                              const bool  bHasMax,
                              const float flMax);

SS_API SSConVar* FindConVar(const char* pName);

SS_API bool RegisterConVarHook(const char* pName);

#endif // SOURCESHARP_CONVAR_H