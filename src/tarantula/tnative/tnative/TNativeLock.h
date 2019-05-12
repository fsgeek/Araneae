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
	void Unlock() {
		ExReleaseResourceAndLeaveCriticalRegion(&m_resource);
	}

	TNativeLock() noexcept { ExInitializeResourceLite(&m_resource); }
	~TNativeLock() noexcept { ExDeleteResourceLite(&m_resource); }

};

