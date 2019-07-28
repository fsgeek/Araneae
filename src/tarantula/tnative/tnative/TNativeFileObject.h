#pragma once
#include <ntifs.h>

class TNativeFileObject
{
	union MemoryTag {
		UCHAR tag[4];
		ULONG tagvalue;
	};

	static const MemoryTag DefaultMemoryTag;

#pragma warning(push)
#pragma warning(disable:26447) // prefast annotations do not throw exceptions; neither does this function
	void* operator new(_In_ size_t size, _In_ void* memory) noexcept {
		UNREFERENCED_PARAMETER(size); return memory;
	};
#pragma warning(pop)

	__drv_allocatesMem(Mem)	void* operator new(_In_ size_t size) noexcept { return ExAllocatePoolWithTag(PagedPool, size, DefaultMemoryTag.tagvalue); };
	void operator delete(__drv_freesMem(mem) void* mem) { ExFreePoolWithTag(mem, DefaultMemoryTag.tagvalue); }


	struct FileContextNonPaged {
		FSRTL_ADVANCED_FCB_HEADER header;
		SECTION_OBJECT_POINTERS sop;
	};

	struct FileContextPaged {
		// TODO: define the paged content here
	};


	FileContextNonPaged *m_FileContextNonPaged = nullptr;
	FileContextPaged* m_FileContextPaged = nullptr;

	TNativeFileObject() noexcept;
	~TNativeFileObject() noexcept;

	PFILE_OBJECT m_FileObject;
public:
	static TNativeFileObject* CreateTNativeFileObject(_In_ PFILE_OBJECT FileObject) noexcept;
	void DeleteTNativeFileObject() noexcept;

	NTSTATUS Read(_Out_writes_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, _In_ ULONG Key = 0) noexcept;
	NTSTATUS Write(_In_reads_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, ULONG Key = 0) noexcept;
	NTSTATUS QueryDirectory(_Out_writes_bytes_(Length) void* Buffer, _In_ ULONG Length, _In_ ULONG FileIndex = 0) noexcept;

	// we don't implement these, so we delete the default implementations.
	TNativeFileObject(const TNativeFileObject&) = delete;
	TNativeFileObject& operator=(const TNativeFileObject& Registry) = delete;
	TNativeFileObject(TNativeFileObject&&) = delete;
	TNativeFileObject& operator=(TNativeFileObject&&) = delete;


};

