#pragma once

#ifdef ENGINE_FRAMEWORK_EXPORTS
#ifdef _WIN32
#define ENGINE_FRAMEWORK_API __declspec(dllexport)
#else
#define ENGINE_FRAMEWORK_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_FRAMEWORK_API __declspec(dllimport)
#else
#define ENGINE_FRAMEWORK_API
#endif
#endif
