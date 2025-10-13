#pragma once

#ifdef ENGINE_ENGINE_EXPORTS
#ifdef _WIN32
#define ENGINE_ENGINE_API __declspec(dllexport)
#else
#define ENGINE_ENGINE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_ENGINE_API __declspec(dllimport)
#else
#define ENGINE_ENGINE_API
#endif
#endif
