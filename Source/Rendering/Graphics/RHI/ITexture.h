#pragma once

class ITexture {
public:
	virtual bool Load(const std::string& path) = 0;
	virtual void Unload() = 0;
};