#include "TNativeFileObject.h"

#pragma warning(push)
#pragma warning(disable:26436 26447) // SAL annotations do not raise 

TNativeGenerator::TNativeGenerator() noexcept
{
	this->m_FileObject = nullptr;
}

TNativeGenerator::~TNativeGenerator() noexcept
{
	if (nullptr != this->m_FileObject) {
		ObDereferenceObject(this->m_FileObject);
		this->m_FileObject = nullptr;
	}
}


TNativeDirectoryObject::TNativeDirectoryObject() noexcept
{
}

TNativeDirectoryObject::~TNativeDirectoryObject() noexcept
{
}

TNativeDirectoryObject* TNativeDirectoryObject::CreateTNativeDirectoryObject(_In_ PFILE_OBJECT FileObject) noexcept
{
	UNREFERENCED_PARAMETER(FileObject);
	return nullptr;
}

_Use_decl_annotations_
NTSTATUS TNativeDirectoryObject::Read(void* Buffer, size_t Offset, ULONG Length, ULONG Flags, ULONG Key) noexcept 
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Offset);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Key);
	return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS TNativeDirectoryObject::Write(_In_reads_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags, ULONG Key) noexcept
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Offset);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Key);
	return STATUS_INVALID_DEVICE_REQUEST;

}

NTSTATUS TNativeDirectoryObject::QueryDirectory(_Out_writes_bytes_(Length) void* Buffer, _In_ ULONG Length, _In_ ULONG FileIndex) noexcept
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(FileIndex);
	return STATUS_INVALID_DEVICE_REQUEST;
}

#pragma warning(pop)