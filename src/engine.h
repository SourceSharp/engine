/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute it 
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 * claim that you wrote the original software. If you use this software in a 
 * product, an acknowledgment in the product documentation would be 
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Version: $Id$
 */

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
