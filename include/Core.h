//
// Created by Bone on 2023/4/24.
//

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
