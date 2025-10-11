#pragma once

#ifdef ENGINE_CORE_EXPORTS
#ifdef _WIN32
#define ENGINE_CORE_API __declspec(dllexport)
#else
#define ENGINE_CORE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_CORE_API __declspec(dllimport)
#else
#define ENGINE_CORE_API
#endif
#endif
