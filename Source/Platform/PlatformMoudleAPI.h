#pragma once

#ifdef ENGINE_PLATFORM_EXPORTS
#ifdef _WIN32
#define ENGINE_PLATFORM_API __declspec(dllexport)
#else
#define ENGINE_PLATFORM_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_PLATFORM_API __declspec(dllimport)
#else
#define ENGINE_PLATFORM_API
#endif
#endif
