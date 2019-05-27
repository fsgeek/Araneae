#pragma once
#include <ntifs.h>
#include "tnative-resource.h"
#include "TNativeLock.h"
#include "memtag.h"

#if 0
union MemoryTag {
	UCHAR tag[4];
	ULONG tagvalue;
};
#endif 0

class TNRegistry
{
	// static const MemoryTag TNRegistryMemoryTag;
	// static const MemoryTag DefaultStringTag;
	// static const MemoryTag DefaultValueEntryTag;
	// static const MemoryTag DefaultKeyEntryTag;

	struct ValueEntry {
		LIST_ENTRY ListEntry;
		UNICODE_STRING Name;
		// must be last field
		union {
			KEY_VALUE_FULL_INFORMATION ValueInfo;
			KEY_BASIC_INFORMATION KeyInfo;
		};
	};

	UNICODE_STRING m_RegistryPath = { 0, 0, nullptr };
	HANDLE m_RegistryHandle = nullptr;
	TNativeLock m_Lock;
	LIST_ENTRY m_ValueList = { &m_ValueList, &m_ValueList };
	LIST_ENTRY m_KeyList = { &m_KeyList, &m_KeyList };

	_Must_inspect_result_ NTSTATUS AddValue(_In_ const KEY_VALUE_FULL_INFORMATION &NewKey) noexcept;
	_Must_inspect_result_ NTSTATUS AddKey(_In_ const KEY_BASIC_INFORMATION &NewKey) noexcept;
	void FreeValueList(void) noexcept;
	void FreeKeyList(void) noexcept;

#pragma warning(push)
#pragma warning(disable:26447) // prefast annotations do not throw exceptions; neither does this function
	void* operator new(_In_ size_t size, _In_ void* memory) noexcept {
		UNREFERENCED_PARAMETER(size); return memory;
	};
#pragma warning(pop)
	__drv_allocatesMem(Mem)	void* operator new(_In_ size_t size) noexcept { return ExAllocatePoolWithTag(PagedPool, size, Tarantula::TNRegistryMemoryTag.tagvalue); };
	void operator delete(__drv_freesMem(mem) void* mem) { ExFreePoolWithTag(mem, Tarantula::TNRegistryMemoryTag.tagvalue); }

	HANDLE GetRegistryHandle(void) noexcept { return m_RegistryHandle; }
	_Must_inspect_result_ NTSTATUS LoadRegistryValues() noexcept;

	TNRegistry() noexcept { InitializeListHead(&m_ValueList); InitializeListHead(&m_KeyList); };
	~TNRegistry() noexcept {};
public:
	// we don't implement these, so we delete the default implementations.
	TNRegistry(const TNRegistry&) = delete;
	TNRegistry& operator=(const TNRegistry& Registry) = delete;
	TNRegistry(TNRegistry&&) = delete;
	TNRegistry& operator=(TNRegistry&&) = delete;

	static _Must_inspect_result_ TNRegistry* CreateTNRegistry(_In_opt_ PCUNICODE_STRING RegistryPath) noexcept;
	static _Must_inspect_result_ TNRegistry* CreateTNRegistry(_In_ TNRegistry *Registry, _In_ PCUNICODE_STRING RegistryPath) noexcept;
	static void DeleteTNRegistry(_In_ _Post_ptr_invalid_ TNRegistry* Registry) noexcept;
	_Must_inspect_result_ NTSTATUS ReadDwordValue(_In_ PCUNICODE_STRING ValueName, _Out_ ULONG& Value);

};

