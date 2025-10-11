#include "Mutex.h"
#include "Logger.hpp"

#ifdef _WIN32

#include <windows.h>
#include <windowsx.h>

Mutex::Mutex() {
	InternalData = CreateMutex(0, 0, 0);
	if (InternalData == nullptr) {
		LOG_ERROR << "Unable to create mutex.";
	}
}

Mutex::~Mutex() {
	if (InternalData == nullptr) {
		return;
	}

	CloseHandle((HANDLE)InternalData);
	InternalData = nullptr;
}

bool Mutex::Lock() {
	if (InternalData == nullptr) {
		return false;
	}

	DWORD Result = WaitForSingleObject((HANDLE)InternalData, INFINITE);
	switch (Result)
	{
		// The thread got ownership of mutex
	case WAIT_OBJECT_0:
		return true;
		// The thread got ownership of an obandoned mutex
	case WAIT_ABANDONED:
		LOG_ERROR << "Mutex lock faield.";
		return false;
	}
	return true;
}

bool Mutex::UnLock() {
	if (InternalData == nullptr) {
		return false;
	}

	int Result = ReleaseMutex((HANDLE)InternalData);
	return Result != 0;	// 0 is failed.
}

#elif defined(__APPLE__)

#include <pthread.h>
#include <errno.h>	

Mutex::Mutex() {
	// Initialize
	pthread_mutexattr_t mutex_attr;
	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_t mutex;
	int result = pthread_mutex_init(&mutex, &mutex_attr);
	if (result != 0) {
		LOG_ERROR << "Mutex creation failure!";
		return;
	}

	// Save off the mutex handle.
	InternalData = Platform::PlatformAllocate(sizeof(pthread_mutex_t), false);
	*(pthread_mutex_t*)InternalData = mutex;

	return;
}

Mutex::~Mutex() {
	if (InternalData == nullptr) {
		return;
	}

	int result = pthread_mutex_destroy((pthread_mutex_t*)InternalData);
	switch (result) {
	case 0:
		// KTRACE("Mutex destroyed.");
		break;
	case EBUSY:
		LOG_ERROR << "Unable to destroy mutex: mutex is locked or referenced.";
		break;
	case EINVAL:
		LOG_ERROR << "Unable to destroy mutex: the value specified by mutex is invalid.";
		break;
	default:
		LOG_ERROR << "An handled error has occurred while destroy a mutex: errno=%i", result;
		break;
	}

	Platform::PlatformFree(InternalData, false);
	InternalData = 0;
}

bool Mutex::Lock() {
	if (InternalData == nullptr) {
		return false;
	}

	// Lock
	int result = pthread_mutex_lock((pthread_mutex_t*)InternalData);
	switch (result) {
	case 0:
		// Success, everything else is a failure.
		// KTRACE("Obtained mutex lock.");
		return true;
	case EOWNERDEAD:
		LOG_ERROR << "Owning thread terminated while mutex still active.";
		return false;
	case EAGAIN:
		LOG_ERROR << "Unable to obtain mutex lock: the maximum number of recursive mutex locks has been reached.";
		return false;
	case EBUSY:
		LOG_ERROR << "Unable to obtain mutex lock: a mutex lock already exists.";
		return false;
	case EDEADLK:
		LOG_ERROR << "Unable to obtain mutex lock: a mutex deadlock was detected.";
		return false;
	default:
		LOG_ERROR << "An handled error has occurred while obtaining a mutex lock: errno=%i", result;
		return false;
	}

	return true;
}

bool Mutex::UnLock() {
	if (InternalData == nullptr) {
		return false;
	}

	int Result = pthread_mutex_unlock((pthread_mutex_t*)InternalData);
	switch (Result) {
	case 0:
		// KTRACE("Freed mutex lock.");
		return true;
	case EOWNERDEAD:
		LOG_ERROR << "Unable to unlock mutex: owning thread terminated while mutex still active.";
		return false;
	case EPERM:
		LOG_ERROR << "Unable to unlock mutex: mutex not owned by current thread.";
		return false;
	default:
		LOG_ERROR << "An handled error has occurred while unlocking a mutex lock: errno=%i", Result;
		return false;
	}
	return false;
}

#endif
