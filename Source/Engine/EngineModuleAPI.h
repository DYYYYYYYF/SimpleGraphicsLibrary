#pragma once

#ifdef ENGINE_ENGINE_EXPORTS
#define ENGINE_ENGINE_API __declspec(dllexport)
#else
#define ENGINE_ENGINE_API __declspec(dllimport)
#endif