//
// Created by Bone on 2023/4/20.
//

#ifndef SAMPLE_MM_CORE_H
#define SAMPLE_MM_CORE_H

#include <unordered_set>
#include <string>

#include "Core.h"


class Core {
public:
    void Load();

    void Unload();

public:
    void RegServerCommand(const char *command);

    void RegClientCommand(const char *command);

private:

#if SOURCE_ENGINE >= SE_ORANGEBOX

    void Hook_ClientCommand(edict_t *pEntity, const CCommand &args);

#else
    void Hook_ClientCommand(edict_t* pEntity);
#endif

private:
    std::unordered_set<std::string> m_ServerCommands;
    std::unordered_set<std::string> m_ClientCommands;
};

extern Core g_Core;


#endif //SAMPLE_MM_CORE_H
