/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SAMPLE_MM_CORE_H
#define SAMPLE_MM_CORE_H

#include "Core.h"

#include <string>
#include <unordered_set>

class Core
{
public:
    void Load();

    void Unload();

public:
    void RegServerCommand(const char* command);

    void RegClientCommand(const char* command);

private:
#if SOURCE_ENGINE >= SE_ORANGEBOX

    void Hook_ClientCommand(edict_t* pEntity, const CCommand& args);

#else
    void Hook_ClientCommand(edict_t* pEntity);
#endif

private:
    std::unordered_set<std::string> m_ServerCommands;
    std::unordered_set<std::string> m_ClientCommands;
};

extern Core g_Core;

#endif // SAMPLE_MM_CORE_H
