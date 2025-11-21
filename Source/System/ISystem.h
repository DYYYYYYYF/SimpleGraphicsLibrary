#pragma once

class ISystem {
public:
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;
};