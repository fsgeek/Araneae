#include "TNativeFileObject.h"

#pragma warning(push)
#pragma warning(disable:26436 26447) // SAL annotations do not raise 


const TNativeFileObject::MemoryTag TNativeFileObject::DefaultMemoryTag = { 'o','f','N','T' };

TNativeFileObject::TNativeFileObject() noexcept
{
	this->m_FileObject = nullptr;
}

TNativeFileObject::~TNativeFileObject() noexcept
{
}

#pragma warning(push)
#pragma warning(disable: )
#pragma warning(disable: 6014) // it's not a leak
#pragma warning(disable: 26400 26409) // it doesn't like me using new
_Use_decl_annotations_
TNativeFileObject* TNativeFileObject::CreateTNativeFileObject(PFILE_OBJECT FileObject) noexcept
{
	TNativeFileObject *tnfo = new TNativeFileObject;
	while (nullptr != tnfo) {
		tnfo->m_FileObject = FileObject;
	}
	return nullptr;
}

void TNativeFileObject::DeleteTNativeFileObject() noexcept
{
	this->m_FileObject = nullptr;
	delete this;
}

_Use_decl_annotations_
NTSTATUS TNativeFileObject::Read(void* Buffer, size_t Offset, ULONG Length, ULONG Flags, ULONG Key) noexcept
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Offset);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Key);

	return STATUS_INVALID_DEVICE_REQUEST;
}

_Use_decl_annotations_
NTSTATUS TNativeFileObject::Write(void* Buffer, size_t Offset, ULONG Length, ULONG Flags, ULONG Key) noexcept
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Offset);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Key);

	return STATUS_INVALID_DEVICE_REQUEST;
}

_Use_decl_annotations_
NTSTATUS TNativeFileObject::QueryDirectory(void* Buffer, ULONG Length, ULONG FileIndex) noexcept
{
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(Length);
	UNREFERENCED_PARAMETER(FileIndex);

	return STATUS_INVALID_DEVICE_REQUEST;
}

#pragma warning(pop)
