#pragma once

#ifdef ENGINE_GRAPHICS_EXPORTS
#ifdef _WIN32
#define ENGINE_GRAPHICS_API __declspec(dllexport)
#else
#define ENGINE_GRAPHICS_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ENGINE_GRAPHICS_API __declspec(dllimport)
#else
#define ENGINE_GRAPHICS_API
#endif
#endif
