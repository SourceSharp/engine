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

#ifndef SOURCESHARP_CORE_H
#define SOURCESHARP_CORE_H

#if defined __WIN32__ || defined _WIN32 || defined WIN32
#define SS_API extern "C" __declspec(dllexport)
#elif defined __GNUC__
#define SS_API extern "C" __attribute__ ((visibility("default")))
#endif

SS_API void RegServerCommand(const char *command);

SS_API void RegClientCommand(const char *command);

#endif //SOURCESHARP_CORE_H
