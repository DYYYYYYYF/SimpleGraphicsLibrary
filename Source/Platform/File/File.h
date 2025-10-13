#pragma once

#include "PlatformMoudleAPI.h"

#include <string>
#include <vector>

#include <fstream>
#include <sstream>

enum class eFileMode {
	Read = 0x1,
	Write = 0x2
};

class File {
public:
	ENGINE_PLATFORM_API File() : IsValid(false){}
	ENGINE_PLATFORM_API File(const std::string& filename);
	ENGINE_PLATFORM_API virtual ~File() {}

public:
	ENGINE_PLATFORM_API std::string GetFilename() const { return FileName; }
	ENGINE_PLATFORM_API std::string GetFullPath() const { return FullPath; }
	ENGINE_PLATFORM_API std::string GetPrePath() const { return PrePath; }
	ENGINE_PLATFORM_API std::string GetFileType() const { return FileType; }
	ENGINE_PLATFORM_API std::string ReadBytes();
	ENGINE_PLATFORM_API bool WriteBytes(const char* source, size_t size, std::ios::openmode mode = std::ios::ate);
	ENGINE_PLATFORM_API bool IsExist();

protected:
	std::string FullPath;
	std::string FileName;
	std::string PrePath;
	std::string FileType;
	bool IsValid;
};
