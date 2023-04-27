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

/* Event */
SS_API bool RegGameEventHook(const char* pName);

class IGameEvent
{
public:
    bool SetInt(const char* pKey, int value);
    int  GetInt(const char* pKey);

    bool SetBool(const char* pKey, bool value);
    bool GetBool(const char* pKey);

    bool  SetFloat(const char* pKey, float value);
    float GetFloat(const char* pKey);

    bool        SetString(const char* pKey, const char* value);
    const char* GetString(const char* pKey);

    void SetBroadcast(bool value);

    const char* GetName();
    bool        GetBroadcast();

private:
    void* m_pEvent;
};

#endif // SOURCESHARP_EVENT_H