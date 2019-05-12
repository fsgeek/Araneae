#pragma once
#include <ntifs.h>

union MemoryTag {
	UCHAR tag[4];
	ULONG tagvalue;
};

class TNRegistry
{

	static const MemoryTag DefaultMemoryTag;
	static const MemoryTag DefaultStringTag;

	UNICODE_STRING m_RegistryPath = { 0, 0, nullptr };
	HANDLE m_RegistryHandle = nullptr;

#pragma warning(push)
#pragma warning(disable:26447) // prefast annotations do not throw exceptions; neither does this function
	void* operator new(_In_ size_t size, _In_ void* memory) noexcept {
		UNREFERENCED_PARAMETER(size); return memory;
	};
#pragma warning(pop)
	__drv_allocatesMem(Mem)	void* operator new(_In_ size_t size) noexcept { return ExAllocatePoolWithTag(PagedPool, size, DefaultMemoryTag.tagvalue); };
	void operator delete(__drv_freesMem(mem) void* mem) { ExFreePoolWithTag(mem, DefaultMemoryTag.tagvalue); }

	HANDLE GetRegistryHandle(void) noexcept { return m_RegistryHandle; }

	TNRegistry() noexcept {};
	~TNRegistry() noexcept {};
public:
	static _Must_inspect_result_ TNRegistry* CreateTNRegistry(_In_opt_ PCUNICODE_STRING RegistryPath) noexcept;
	static _Must_inspect_result_ TNRegistry* CreateTNRegistry(_In_ TNRegistry &Registry, _In_ PCUNICODE_STRING RegistryPath) noexcept;
	static void DeleteTNRegistry(_In_ _Post_ptr_invalid_ TNRegistry* Registry) noexcept;
	// we don't implement these, so we delete the default implementations.
	TNRegistry(const TNRegistry&) = delete;
	TNRegistry& operator=(const TNRegistry& Registry) = delete;
	TNRegistry(TNRegistry&&) = delete;
	TNRegistry& operator=(TNRegistry&&) = delete;
};

