#pragma once

#ifdef ENGINE_PLATFORM_EXPORTS
#define ENGINE_PLATFORM_API __declspec(dllexport)
#else
#define ENGINE_PLATFORM_API __declspec(dllimport)
#endif