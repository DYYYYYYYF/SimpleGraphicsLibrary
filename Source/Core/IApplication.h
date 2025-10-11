#pragma once

#include <string>

class IApplication {
public:
	virtual ~IApplication() = default;

public:
	virtual bool Initialize() = 0;
	virtual void Tick() = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;
	virtual const std::string& GetName() { return AppName_; }

protected:
	std::string AppName_;
};