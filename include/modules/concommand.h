/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_CONCOMMAND_H
#define SOURCESHARP_CONCOMMAND_H

#include "_export.h"

/* Console Command */
SS_API void RegServerCommand(const char *command);
SS_API void RegClientCommand(const char *command);

#endif //SOURCESHARP_CONCOMMAND_H