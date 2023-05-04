/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_ENGINE_SHARED_H
#define SOURCESHARP_ENGINE_SHARED_H
#include <platform.h>

class ISourceSharpModule
{
     void OnLoad();
     void OnShutdown();
};

#endif // SOURCESHARP_ENGINE_SHARED_H
