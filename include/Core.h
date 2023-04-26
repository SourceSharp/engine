/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_CORE_H
#define SOURCESHARP_CORE_H

#if defined __WIN32__ || defined _WIN32 || defined WIN32
#define SS_API extern "C" __declspec(dllexport)
#elif defined __GNUC__
#define SS_API extern "C" __attribute__ ((visibility("default")))
#endif

/* Console Command */
SS_API void RegServerCommand(const char *command);
SS_API void RegClientCommand(const char *command);

/* Event */
SS_API bool         Event_SetInt(const char* pKey, int value);
SS_API int          Event_GetInt(void* pEvent, const char* pKey);
SS_API bool         Event_SetBool(const char* pKey, bool value);
SS_API bool         Event_GetBool(void* pEvent, const char* pKey);
SS_API bool         Event_SetFloat(const char* pKey, float value);
SS_API float        Event_GetFloat(void* pEvent, const char* pKey);
SS_API bool         Event_SetString(const char* pKey, const char* value);
SS_API const char*  Event_GetString(const char* pKey, float value);
SS_API void         Event_SetBroadcast(void* pEvent, bool value);

#endif //SOURCESHARP_CORE_H
