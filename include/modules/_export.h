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
#    define SS_INLINE __forceinline
#elif defined __GNUC__
#    define SS_API extern "C" __attribute__((visibility("default")))
#    define SS_CLASS_EXPORT __attribute__((visibility("default")))
#    define SS_INLINE inline
#endif

#define DECL_CLASS_MEMBER_EXPORT_GET(class, member, ret) \
    SS_API ret class##Get##member(const class* ptr)

#define DECL_CLASS_MEMBER_EXPORT_SET(class, member, type) \
    SS_API void class##Set##member(const class* ptr, type val)

#define DECL_CLASS_MEMBER_FUNCTION_p0(class, func, ret) \
    SS_API ret class##Call##func(const class* ptr)

#endif // SOURCESHARP_EXPORT_H
