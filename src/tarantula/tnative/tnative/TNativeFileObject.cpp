#include "TNativeFileObject.h"


TNativeDirectoryObject::TNativeDirectoryObject()
{
}

TNativeDirectoryObject::~TNativeDirectoryObject()
{
}

TNativeDirectoryObject* TNativeDirectoryObject::CreateTNativeDirectoryObject(_In_ PFILE_OBJECT FileObject) noexcept
{
	UNREFERENCED_PARAMETER(FileObject);
	return nullptr;
}

NTSTATUS TNativeDirectoryObject::Read(_Out_writes_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, _In_ ULONG Key = 0) noexcept 
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Offset);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Key);
	return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS Write(_In_reads_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, ULONG Key = 0) noexcept
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Offset);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Key);
	return STATUS_INVALID_DEVICE_REQUEST;

}

NTSTATUS QueryDirectory(_Out_writes_bytes_(Length) void* Buffer, _In_ ULONG Length, _In_ ULONG FileIndex = 0) noexcept
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(FileIndex);
	return STATUS_INVALID_DEVICE_REQUEST;
}
