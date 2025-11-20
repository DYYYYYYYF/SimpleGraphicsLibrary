#pragma once

#include "CoreModuleAPI.h"

/**
 * A mutex to be used for synchronization purposes. A mutex (or mutual exclusion)
 * is used to limit access to a resource when there are multiple threads of
 * execution around that resource.
 */
class Mutex {
public:
	ENGINE_CORE_API Mutex();
	ENGINE_CORE_API virtual ~Mutex();

	/**
	 * Creates a mutex lock.
	 * @returns True if locked successfully.
	 */
	ENGINE_CORE_API bool Lock();

	/**
	 * Unlock the mutex.
	 * @returns True if unlocked successfully.
	 */
	ENGINE_CORE_API bool UnLock();

public:
	void* InternalData;
};

class MutexGuard {
public:
	explicit MutexGuard(Mutex& mutex)
		: Mutex_(mutex), IsLocked_(false)
	{
		IsLocked_ = Mutex_.Lock();
	}

	~MutexGuard() {
		if (IsLocked_) {
			Mutex_.UnLock();
		}
	}

	// 禁止拷贝构造和赋值，防止资源重复释放
	MutexGuard(const MutexGuard&) = delete;
	MutexGuard& operator=(const MutexGuard&) = delete;

private:
	Mutex& Mutex_;
	bool IsLocked_;
};