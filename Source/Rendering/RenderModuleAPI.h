#pragma once

#ifdef ENGINE_RENDERING_EXPORTS
#ifdef _WIN32
#define ENGINE_RENDERING_API __declspec(dllexport)
#else
#define ENGINE_RENDERING_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_RENDERING_API __declspec(dllimport)
#else
#define ENGINE_RENDERING_API
#endif
#endif
