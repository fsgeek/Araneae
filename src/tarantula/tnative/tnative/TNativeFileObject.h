#pragma once
#include <ntifs.h>

class TNativeGenerator
{
	//
	// A "generator" is anything we manage that can generate data
	//
	// For Windows, there are really two APIs for generating data:
	//  (1) the usual read/write interface
	//  (2) the directory enumeration interface
	//
	//

	PFILE_OBJECT *m_FileObject; // we always need a file object in order to do anything

public:
	TNativeGenerator() noexcept;
	virtual ~TNativeGenerator() noexcept;

#pragma warning(push)
#pragma warning(disable:26436 26447) // SAL annotations do not raise 
	virtual NTSTATUS Read(_Out_writes_bytes_(Length) void *Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, _In_ ULONG Key = 0) noexcept {
		//
		// Default is "not implemented"
		//
		/*
		struct {
			ULONG Length;
            ULONG POINTER_ALIGNMENT Key;
#if defined(_WIN64)
            ULONG Flags;
#endif
            LARGE_INTEGER ByteOffset;
        } Read;
		*/
		UNREFERENCED_PARAMETER(Buffer);
		UNREFERENCED_PARAMETER(Offset);
		UNREFERENCED_PARAMETER(Length);
		UNREFERENCED_PARAMETER(Flags);
		UNREFERENCED_PARAMETER(Key);
		return STATUS_INVALID_DEVICE_REQUEST;
	};

	virtual NTSTATUS Write(_In_reads_bytes_(Length) void *Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, ULONG Key = 0) noexcept {
		UNREFERENCED_PARAMETER(Buffer);
		UNREFERENCED_PARAMETER(Offset);
		UNREFERENCED_PARAMETER(Length);
		UNREFERENCED_PARAMETER(Flags);
		UNREFERENCED_PARAMETER(Key);
		return STATUS_INVALID_DEVICE_REQUEST;
	}

	virtual NTSTATUS QueryDirectory(_Out_writes_bytes_(Length) void *Buffer, _In_ ULONG Length, _In_ ULONG FileIndex = 0) noexcept {
		/*
		struct {
			ULONG Length;
			PUNICODE_STRING FileName;
			FILE_INFORMATION_CLASS FileInformationClass;
			ULONG POINTER_ALIGNMENT FileIndex;
		} QueryDirectory;
		*/
		UNREFERENCED_PARAMETER(Buffer);
		UNREFERENCED_PARAMETER(Length);
		UNREFERENCED_PARAMETER(FileIndex);
		return STATUS_INVALID_DEVICE_REQUEST;
	}

	// TODO: there are other functions common to all generators: EAs, ACLs, Information.
#pragma warning(pop)
};	

class TNativeFileObject : public TNativeGenerator
{
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

public:
	static TNativeFileObject* CreateTNativeFileObject(_In_ PFILE_OBJECT FileObject) noexcept;

	NTSTATUS Read(_Out_writes_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, _In_ ULONG Key = 0) noexcept override;
	NTSTATUS Write(_In_reads_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, ULONG Key = 0) noexcept override;
	NTSTATUS QueryDirectory(_Out_writes_bytes_(Length) void* Buffer, _In_ ULONG Length, _In_ ULONG FileIndex = 0) noexcept override;
};

class TNativeDirectoryObject : public TNativeGenerator
{
	FSRTL_ADVANCED_FCB_HEADER m_Header;


	TNativeDirectoryObject() noexcept;
	~TNativeDirectoryObject() noexcept;

public:
	static TNativeDirectoryObject* CreateTNativeDirectoryObject(_In_ PFILE_OBJECT FileObject) noexcept;

	NTSTATUS Read(_Out_writes_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, _In_ ULONG Key = 0) noexcept override;
	NTSTATUS Write(_In_reads_bytes_(Length) void* Buffer, _In_ size_t Offset, _In_ ULONG Length, _In_ ULONG Flags = 0, ULONG Key = 0) noexcept override;
	NTSTATUS QueryDirectory(_Out_writes_bytes_(Length) void* Buffer, _In_ ULONG Length, _In_ ULONG FileIndex = 0) noexcept override;

};