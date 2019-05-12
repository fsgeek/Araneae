//
// This is the CPP runtime initialization interface
//
// (C) Copyright 2019 Tony Mason, All Rights Reserved
//
#pragma once

#include <ntddk.h>

//
// So the driver using this calls pre_init at the beginning of DriverEntry.  This will set up
// any runtime support needed, as well as call the constructors for global and static objects.
// To ensure proper termination, the driver calls post_init at the end of DriverEntry.  This
// will override the unload function in the driver object.  On Unload, this will chain (so the
// runtime will call the driver's unload function, and then do its own cleanup).
//
extern "C" {
	NTSTATUS cpp_rt_pre_init(void) noexcept;
	NTSTATUS cpp_rt_post_init(PDRIVER_OBJECT DriverObject, const PUNICODE_STRING RegistryPath) noexcept;
}

#if 0
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
	void* __cdecl malloc(_In_ __drv_strictTypeMatch(__drv_typeExpr) POOL_TYPE pool, size_t length, ULONG tag);

__drv_allocatesMem(Mem)
_Post_maybenull_
_Must_inspect_result_
_Post_writable_byte_size_(length)
void* __cdecl malloc(_In_ size_t length);

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
	void* __cdecl malloc(POOL_TYPE pool, size_t length);

_IRQL_requires_max_(DISPATCH_LEVEL)
void __cdecl free(_In_ __drv_freesMem(Mem)  void* buffer, _In_ ULONG tag);

_IRQL_requires_max_(DISPATCH_LEVEL)
void __cdecl free(_In_ __drv_freesMem(Mem)  void* buffer);

void* __cdecl operator new(size_t size);
void* __cdecl operator new(size_t size, POOL_TYPE pool);
void* __cdecl operator new(size_t size, POOL_TYPE pool, ULONG tag);
void* __cdecl operator new(size_t size, void* buffer);

void __cdecl operator delete(void* pVoid, ULONG tag);
void __cdecl operator delete(void* pVoid);
#endif // 0
