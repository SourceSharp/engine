/**
 * ======================================================
 * SourceSharp - Engine Layer.
 * Copyright (C) 2023-Present SourceSharp Dev Team.
 * All rights reserved.
 * ======================================================
 */

#ifndef SOURCESHARP_EXPORT_H
#define SOURCESHARP_EXPORT_H

#if defined __WIN32__ || defined _WIN32 || defined WIN32
#    define SS_API extern "C" __declspec(dllexport)
#    define SS_CLASS_EXPORT __declspec(dllexport)
#elif defined __GNUC__
#    define SS_API extern "C" __attribute__((visibility("default")))
#    define SS_CLASS_EXPORT __attribute__((visibility("default")))
#endif



#endif // SOURCESHARP_EXPORT_H
