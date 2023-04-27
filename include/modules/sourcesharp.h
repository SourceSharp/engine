/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_SOURCESHARP_H
#define SOURCESHARP_SOURCESHARP_H

#include "_export.h"

SS_API const char* GetGamePath();

SS_API void LogError(const char* pMessage);
SS_API void LogMessage(const char* pMessage);

SS_API int GetMaxClients();
SS_API int GetMaxHumanPlayers();
SS_API int GetEngineVersion();

SS_API void ExecuteServerCommand(const char* pCommand);
SS_API void InsertServerCommand(const char* pCommand);
SS_API void ServerExecute();

#endif // SOURCESHARP_SOURCESHARP_H