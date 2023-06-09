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

class SSConVar;

DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, Name, const char*);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, Default, const char*);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, Description, const char*);

SS_API int  SSConVarGetFlags(const SSConVar* pVar);
SS_API void SSConVarAddFlags(const SSConVar* pVar, int nFlags);

// value
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, Int, int);
DECL_CLASS_MEMBER_EXPORT_SET(SSConVar, Int, int);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, Bool, bool);
DECL_CLASS_MEMBER_EXPORT_SET(SSConVar, Bool, bool);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, Float, float);
DECL_CLASS_MEMBER_EXPORT_SET(SSConVar, Float, float);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, String, const char*);
DECL_CLASS_MEMBER_EXPORT_SET(SSConVar, String, const char*);

// bound
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, MinValue, float);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, MaxValue, float);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, HasMin, bool);
DECL_CLASS_MEMBER_EXPORT_GET(SSConVar, HasMax, bool);
SS_API void SSConVarSetBound(const SSConVar* pVar,
                             bool            bHasMin,
                             float           flMin,
                             bool            bHasMax,
                             float           flMax);
// function
SS_API void SSConVarRevert(const SSConVar* pVar);
SS_API bool SSConVarReplicateToPlayers(const SSConVar* pVar, const int pPlayers[], int nPlayers);

SS_API SSConVar* CreateConVar(const char* pName,
                              const char* pDECLValue,
                              const char* pDescription,
                              int         nFlags,
                              bool        bHasMin,
                              float       flMin,
                              bool        bHasMax,
                              float       flMax);

SS_API SSConVar* FindConVar(const char* pName);

SS_API bool RegisterConVarHook(const char* pName);

#endif // SOURCESHARP_CONVAR_H