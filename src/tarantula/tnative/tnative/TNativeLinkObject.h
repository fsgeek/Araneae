#pragma once

#include <ntifs.h>
#include "memtag.h"
#include "TNativeFileObject.h"

//
// In Windows, a file may have multiple possible names for reaching that file.  This is commonly referred to
// as a link.  Since some operations are done against open instances that require knowing which name is being used, 
// we track the specific name.
//

class TNativeLinkObject
{
	static const MemoryTag DefaultMemoryTag;
	static const MemoryTag LinkNameMemoryTag;
	UNICODE_STRING m_LinkName = { 0, 0, nullptr };
	TNativeFileObject *m_FileObject = nullptr;
	TNativeFileObject *m_DirectoryObject = nullptr;

#pragma warning(push)
#pragma warning(disable: 26447) // the annotation does not raise
	void* operator new(_In_ size_t size, _Out_writes_bytes_(size) void* memory) noexcept {
		UNREFERENCED_PARAMETER(size); return memory;
	};
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:26447) // prefast annotations do not throw exceptions; neither does this function
	__drv_allocatesMem(Mem)	void* operator new(_In_ size_t size) noexcept { 
		return ExAllocatePoolWithTag(PagedPool, size, DefaultMemoryTag.tagvalue); 
	};
#pragma warning(pop)

	void operator delete(__drv_freesMem(mem) void* mem) noexcept { ExFreePoolWithTag(mem, DefaultMemoryTag.tagvalue); };

	TNativeLinkObject() noexcept {
		m_LinkName.Length = 0;
		m_LinkName.MaximumLength = 0;
		m_LinkName.Buffer = nullptr;
	}

protected:
	~TNativeLinkObject() {
		if (nullptr != m_LinkName.Buffer) {
			ExFreePoolWithTag(m_LinkName.Buffer, LinkNameMemoryTag.tagvalue);
			m_LinkName.Buffer = nullptr;
		}
	}

public:
	static TNativeLinkObject* CreateLink(PCUNICODE_STRING LinkName) noexcept {
#pragma warning(suppress:26400 26409) // we have to call new directly, we're in the kernel.
		TNativeLinkObject* lo = new TNativeLinkObject;

		while (nullptr != lo) {
			// make enough space that we have room to null terminate (not required, but easier)
			lo->m_LinkName.Buffer = static_cast<PWCH>(ExAllocatePoolWithTag(PagedPool, LinkName->Length + sizeof(WCHAR), LinkNameMemoryTag.tagvalue));
			if (nullptr == lo->m_LinkName.Buffer) {
				delete lo;
				lo = nullptr;
				break;
			}

			lo->m_LinkName.Length = LinkName->Length;
			lo->m_LinkName.MaximumLength = LinkName->Length + sizeof(WCHAR);
#pragma warning(suppress:26486) // If the UNICODE_STRING is pointing to an invalid buffer, we'll crash.  live with it.
			RtlCopyMemory(lo->m_LinkName.Buffer, LinkName->Buffer, LinkName->Length);
#pragma warning(suppress:26481) // we don't have "span" in this environment
			lo->m_LinkName.Buffer[lo->m_LinkName.Length / sizeof(WCHAR)];

			// we now have a valid object
			break;
		}

		return lo;
	};

	static TNativeLinkObject* CreateLink(PWCHAR LinkName) noexcept {
		UNICODE_STRING ln;

#pragma warning(push)
#pragma warning(disable:26472) // we know this is a static cast for arithmetic conversion; it's fine in this case.
		ln.Length = static_cast<USHORT>(sizeof(WCHAR) * wcslen(LinkName));
		ln.MaximumLength = static_cast<USHORT>(ln.Length + sizeof(WCHAR));
#pragma warning(pop)
		ln.Buffer = LinkName;
		return CreateLink(&ln);
	};

	NTSTATUS AddFileObject(TNativeFileObject* FileObject) noexcept {
		m_FileObject = FileObject;
		return STATUS_SUCCESS;
	}
	NTSTATUS AddParentDirectoryObject(TNativeFileObject* ParentDirectoryObject) noexcept {
		m_DirectoryObject = ParentDirectoryObject;
		return STATUS_SUCCESS;
	}


	// we don't implement these, so we delete the default implementations.
	TNativeLinkObject(const TNativeLinkObject&) = delete;
	TNativeLinkObject& operator=(const TNativeLinkObject& Registry) = delete;
	TNativeLinkObject(TNativeLinkObject&&) = delete;
	TNativeLinkObject& operator=(TNativeLinkObject&&) = delete;

};
