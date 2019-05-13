#pragma once
#include <ntifs.h>

class TNativeLock
{
	union MemoryTag {
		UCHAR tag[4];
		ULONG tagvalue;
	};

	static const MemoryTag DefaultMemoryTag;

	ERESOURCE m_resource;

#pragma warning(push)
#pragma warning(disable:26447) // prefast annotations do not throw exceptions; neither does this function
	void* operator new(_In_ size_t size, _In_ void* memory) noexcept {
		UNREFERENCED_PARAMETER(size); return memory;
	};
#pragma warning(pop)
	__drv_allocatesMem(Mem)	void* operator new(_In_ size_t size) noexcept { return ExAllocatePoolWithTag(PagedPool, size, DefaultMemoryTag.tagvalue); };
	void operator delete(__drv_freesMem(mem) void* mem) { ExFreePoolWithTag(mem, DefaultMemoryTag.tagvalue); }

public:
	PERESOURCE GetLock() noexcept { return &m_resource; }

	// TODO: annotate this properly - it's an ugly one
	/* _IRQL_requires_max_(APC_LEVEL)
		_Acquires_lock_(_Global_critical_region_)
		NTKERNELAPI
		PVOID
		ExEnterCriticalRegionAndAcquireResourceExclusive(
			_Inout_ _Requires_lock_not_held_(*_Curr_) _Acquires_exclusive_lock_(*_Curr_)
			PERESOURCE Resource
		);
	*/
	_IRQL_requires_max_(APC_LEVEL)
	_Acquires_lock_(_Global_critical_region_)
	_Requires_lock_not_held_(m_resource) 
	_When_(Exclusive == true, _Acquires_exclusive_lock_(m_resource))
	_When_(Exclusive != true, _Acquires_shared_lock_(m_resource))
	void Lock(_In_ bool Exclusive = true) noexcept {
		if (Exclusive) {
			ExEnterCriticalRegionAndAcquireResourceExclusive(&m_resource);
		}
		else {
			ExEnterCriticalRegionAndAcquireResourceShared(&m_resource);
		}
	}

	bool TryToLock(bool Exclusive = true) noexcept {
		bool result = false;
		KeEnterCriticalRegion();
		if (Exclusive) {
			if (ExAcquireResourceExclusiveLite(&m_resource, false)) {
				result = true;
			};
		}
		else {
			if (ExAcquireResourceSharedLite(&m_resource, false)) {
				result = true;
			}
		}
		if (!result) {
			KeLeaveCriticalRegion();
		}
		return result;
	}

	_IRQL_requires_max_(DISPATCH_LEVEL)
		//_Requires_lock_held_(_GlobalCriticalRegion_)
		_Requires_lock_held_(m_resource)
		_Releases_lock_(_Global_critical_region_)
		_Releases_lock_(m_resource)
	void Unlock() {
		ExReleaseResourceAndLeaveCriticalRegion(&m_resource);
	}

	TNativeLock() noexcept { ExInitializeResourceLite(&m_resource); }
	~TNativeLock() noexcept { ExDeleteResourceLite(&m_resource); }

	// we don't implement these, so we delete the default implementations.
	TNativeLock(const TNativeLock&) = delete;
	TNativeLock& operator=(const TNativeLock& Registry) = delete;
	TNativeLock(TNativeLock&&) = delete;
	TNativeLock& operator=(TNativeLock&&) = delete;


};

