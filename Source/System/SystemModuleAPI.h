#pragma once

#ifdef ENGINE_SYSTEM_EXPORTS
#ifdef _WIN32
#define ENGINE_SYSTEM_API __declspec(dllexport)
#else
#define ENGINE_SYSTEM_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_SYSTEM_API __declspec(dllimport)
#else
#define ENGINE_SYSTEM_API
#endif
#endif
