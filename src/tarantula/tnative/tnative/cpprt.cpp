// This is the CPP runtime support
//
// (C) Copyright 2019 Tony Mason, All Rights Reserved
//
#include <ntddk.h>
#include <sal.h>

// I'm doing some unsavory things here _and_ using kernel annotations for things that are now considered forbidden
// so I have to mask off several warnings.  Some of this is because I'm on the boundary between C++ and Windows OS
// functionality.
#pragma warning(disable:28285 28218 28160 26408 26447)

static const char* default_tag_str = "lksW";
#pragma warning(suppress:26493)
static const ULONG default_tag = *(ULONG*)default_tag_str;

__drv_allocatesMem(Mem)
_When_((PoolType& PagedPool) != 0, _IRQL_requires_max_(APC_LEVEL))
_When_((PoolType& PagedPool) == 0, _IRQL_requires_max_(DISPATCH_LEVEL))
_When_((PoolType& NonPagedPoolMustSucceed) != 0,
	__drv_reportError("Must succeed pool allocations are forbidden. "
		"Allocation failures cause a system crash"))
	_When_((PoolType& (NonPagedPoolMustSucceed |
		POOL_RAISE_IF_ALLOCATION_FAILURE)) == 0,
		_Post_maybenull_ _Must_inspect_result_)
	_When_((PoolType& (NonPagedPoolMustSucceed |
		POOL_RAISE_IF_ALLOCATION_FAILURE)) != 0,
		_Post_notnull_)
	_Post_writable_byte_size_(length)
void *__cdecl malloc(_In_ __drv_strictTypeMatch(__drv_typeExpr) POOL_TYPE pool, size_t length, ULONG tag)
{
	return ExAllocatePoolWithTag(pool, length, tag);
}

__drv_allocatesMem(Mem)
_Post_maybenull_
_Must_inspect_result_
_Post_writable_byte_size_(length)
void *__cdecl malloc(_In_ size_t length)
{
	return malloc(PagedPool, length, default_tag);
}

__drv_allocatesMem(Mem)
_When_((PoolType& PagedPool) != 0, _IRQL_requires_max_(APC_LEVEL))
_When_((PoolType& PagedPool) == 0, _IRQL_requires_max_(DISPATCH_LEVEL))
_When_((PoolType& NonPagedPoolMustSucceed) != 0,
	__drv_reportError("Must succeed pool allocations are forbidden. "
		"Allocation failures cause a system crash"))
	_When_((PoolType& (NonPagedPoolMustSucceed |
		POOL_RAISE_IF_ALLOCATION_FAILURE)) == 0,
		_Post_maybenull_ _Must_inspect_result_)
	_When_((PoolType& (NonPagedPoolMustSucceed |
		POOL_RAISE_IF_ALLOCATION_FAILURE)) != 0,
		_Post_notnull_)
	_Post_writable_byte_size_(length)
void *__cdecl malloc(POOL_TYPE pool, size_t length)
{
	return malloc(pool, length, default_tag);
}

_IRQL_requires_max_(DISPATCH_LEVEL)
void __cdecl free(_In_ __drv_freesMem(Mem)  void *buffer, _In_ ULONG tag)
{
	if (nullptr != buffer) {
		ExFreePoolWithTag(buffer, tag);
	}
}

_IRQL_requires_max_(DISPATCH_LEVEL)
void __cdecl free(_In_ __drv_freesMem(Mem)  void* buffer)
{
	free(buffer, default_tag);
}


void *__cdecl operator new(size_t size)
{
	return malloc(PagedPool, size, default_tag);
}

void *__cdecl operator new(size_t size, POOL_TYPE pool)
{
	return malloc(pool, size, default_tag);
}

void *__cdecl operator new(size_t size, POOL_TYPE pool, ULONG tag)
{
	return malloc(pool, size, tag);
}


// placement new
void *__cdecl operator new(size_t size, void *buffer)
{
	UNREFERENCED_PARAMETER(size);

	return buffer;
}

void __cdecl operator delete(void* pVoid, ULONG tag)
{
	free(pVoid, tag);
}

void __cdecl operator delete(void* buffer, size_t length)
{
	UNREFERENCED_PARAMETER(length);
	free(buffer);
}

void __cdecl operator delete(void *pVoid)
{
	free(pVoid, default_tag);
}

typedef void(__cdecl* _PVFV)(void);

typedef struct _CPPRT_EXIT_ENTRY {
	LIST_ENTRY ListEntry;
	_PVFV	   ExitFunction;
} CPPRT_EXIT_ENTRY, *PCPPRT_EXIT_ENTRY;

LIST_ENTRY CpprtExitListHead;


//
// onexit is a standard (runtime) routine for requesting a callback at runtime termination
//
_PVFV __cdecl onexit(_PVFV ExitFunction)

{
#pragma warning(suppress:26493) // we're on the boundary here
	PCPPRT_EXIT_ENTRY exitEntry = (CPPRT_EXIT_ENTRY *)ExAllocatePoolWithTag(PagedPool, sizeof(CPPRT_EXIT_ENTRY), 'ppcW');

	if (nullptr == exitEntry) {
		return nullptr;
	}
	exitEntry->ExitFunction = ExitFunction;
	InsertHeadList(&CpprtExitListHead, &exitEntry->ListEntry); // push onto stack

	return ExitFunction;
}

_Function_class_(DRIVER_UNLOAD)
static void (*CppRtSavedDriverUnload)(PDRIVER_OBJECT) = nullptr;

static void CppRTDriverUnload(PDRIVER_OBJECT DriverObject)
{
	PCPPRT_EXIT_ENTRY exitEntry = nullptr;

	// First, cleanup the driver
#pragma warning(suppress:28023 28175) // it complains about annotating this as a driver unload function, but it is annotated as such.
	DriverObject->DriverUnload = CppRtSavedDriverUnload;
	if (nullptr != CppRtSavedDriverUnload) {
		CppRtSavedDriverUnload(DriverObject);
	}

	// Now shut down the runtime
	while (!IsListEmpty(&CpprtExitListHead)) {
#pragma warning(suppress:26481)
		exitEntry = CONTAINING_RECORD(RemoveHeadList(&CpprtExitListHead), CPPRT_EXIT_ENTRY, ListEntry);

		if (nullptr != exitEntry && nullptr != exitEntry->ExitFunction) {
			__try {
				(exitEntry->ExitFunction)();
			}
#pragma warning(suppress:6320 6322) // yes, it might mask exceptions - that's the *point*
			__except (EXCEPTION_EXECUTE_HANDLER) {
				// ignore for now
			}
		}

		ExFreePoolWithTag(exitEntry, 'ppcW');
		exitEntry = nullptr;
	}
}

extern "C" {
#if defined(_IA64_) || defined(_AMD64_)
#pragma section(".CRT$XCA",long,read)
	__declspec(allocate(".CRT$XCA")) void(*__crtXca[1])(void) = { nullptr };
#pragma section(".CRT$XCZ",long,read)
	__declspec(allocate(".CRT$XCZ")) void(*__crtXcz)(void) = { nullptr };
#pragma data_seg()
#else
#pragma data_seg(".CRT$XCA")
	void(*__crtXca[1])(void) = { nullptr };
#pragma data_seg(".CRT$XCZ")
	void(*__crtXcz[1])(void) = { nullptr };
#pragma data_seg()
#endif

	NTSTATUS cpp_rt_pre_init(void) noexcept
	{
		InitializeListHead(&CpprtExitListHead);

#pragma warning(push)
#pragma warning(disable: 26489) // if these pointers are NULL we will blow up, and deservedly so because the compiler screwed up
#pragma warning(disable: 26485) // "pointer decay" isn't going to happen here
		for (_PVFV* initializer = (_PVFV*)__crtXca; nullptr != initializer && initializer < (_PVFV*)__crtXcz; initializer++) {
			if (nullptr != *initializer) {
				__try {
					(**initializer)();
				}
#pragma warning(suppress:6320) // yes, the purpose of this is to catch errors that simply should not be happening
				__except (EXCEPTION_EXECUTE_HANDLER) {
					return STATUS_ACCESS_VIOLATION;
				}
			}
		}
#pragma warning(pop)

		return STATUS_SUCCESS;
	}


#pragma warning(suppress:26461) // the analyzer complains that I should use const for the Registry
	NTSTATUS cpp_rt_post_init(PDRIVER_OBJECT DriverObject, const PUNICODE_STRING RegistryPath) noexcept
	{
#pragma warning(suppress: 28023 28175) // we don't get to control the annotation on someone else's function.
		CppRtSavedDriverUnload = DriverObject->DriverUnload;
#pragma warning(suppress: 28023 28175) // this kind of driver manipulates the unload function
		DriverObject->DriverUnload = CppRTDriverUnload;
		UNREFERENCED_PARAMETER(RegistryPath);
		return STATUS_SUCCESS;
	}
}