/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_EVENT_H
#define SOURCESHARP_EVENT_H

#include "_export.h"

class SSGameEvent
{
public:
    const char* GetName();

    bool SetInt(const char* pKey, int value);
    int  GetInt(const char* pKey);

    bool SetBool(const char* pKey, bool value);
    bool GetBool(const char* pKey);

    bool  SetFloat(const char* pKey, float value);
    float GetFloat(const char* pKey);

    bool        SetString(const char* pKey, const char* value);
    const char* GetString(const char* pKey);

    bool        GetBroadcast();
    bool        SetBroadcast(bool value);

    void Cancel();

private:
    class IGameEvent;
    IGameEvent* m_pEvent;
};

SS_API bool            RegGameEventHook(const char* pName);
SS_API SSGameEvent*    CreateGameEvent(const char* pName, bool bBroadcast);

#endif // SOURCESHARP_EVENT_H