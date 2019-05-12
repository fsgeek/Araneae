#pragma once

//
// This is a wrapper around include/wil/resource.h
//
#pragma warning(push)
#pragma warning(disable: 26497) // include\wil\common.h(577) : warning C26497 : The function 'wil::verify_bool<bool,0>' could be marked constexpr if compile - time evaluation is desired(f.4).
#pragma warning(disable: 26440) // include\wil\common.h(577) : warning C26440 : Function 'wil::verify_bool<bool,0>' can be declared 'noexcept' (f.6).
#pragma warning(disable: 28157) // include\wil\resource.h(5494) : warning C28157 : The IRQL in 'spinLockSavedIrql' was never restored.
#pragma warning(disable: 28158) // include\wil\resource.h(5522) : warning C28158 : No IRQL was saved into 'return'.
#pragma warning(disable: 26473) // include\wil\resource.h(124) : warning C26473 : Don't cast between pointer types where the source type and the target type are the same (type.1).
#pragma warning(disable: 26493) // include\wil\resource.h(124) : warning C26493 : Don't use C-style casts (type.4).
#pragma warning(disable: 28167) // include\wil\resource.h(125) : warning C28167 : The function 'close' changes the IRQLand does not restore the IRQL before it exits.It should be annotated to reflect the change or the IRQL should be restored.IRQL was last set at line 125.
#pragma warning(disable: 26447) // include\wil\resource.h(124) : warning C26447 : The function is declared 'noexcept' but calls function 'operator unsigned long *()' which may throw exceptions(f.6).
#pragma warning(disable: 26473) // include\wil\wistd_type_traits.h(1929) : warning C26473 : Don't cast between pointer types wh
#include "include/wil/resource.h"
#pragma warning(pop)

// TODO: work on extending the WIL model to support devices
namespace wil {

	namespace details
	{
#pragma warning(push)
#pragma warning(disable:26461) // it can be marked const NOW, but that breaks when we use the input parameter
#pragma warning(disable:28196) // the device object IS null on failure
		inline 
		_IRQL_requires_max_(APC_LEVEL)
		_Ret_range_(<= , 0)
		NTKERNELAPI
		NTSTATUS
			WilIoCreateDevice(
				_In_  PDRIVER_OBJECT DriverObject,
				_In_  ULONG DeviceExtensionSize,
				_In_opt_ PUNICODE_STRING DeviceName,
				_In_  DEVICE_TYPE DeviceType,
				_In_  ULONG DeviceCharacteristics,
				_In_  BOOLEAN Exclusive,
				_Outptr_result_nullonfailure_
				_At_(*DeviceObject,
					__drv_allocatesMem(Mem)
					_When_((((_In_function_class_(DRIVER_INITIALIZE))
						|| (_In_function_class_(DRIVER_DISPATCH)))),
						__drv_aliasesMem))
				PDEVICE_OBJECT* DeviceObject
			) {
			UNREFERENCED_PARAMETER(DriverObject);
			UNREFERENCED_PARAMETER(DeviceExtensionSize);
			UNREFERENCED_PARAMETER(DeviceName);
			UNREFERENCED_PARAMETER(DeviceType);
			UNREFERENCED_PARAMETER(DeviceCharacteristics);
			UNREFERENCED_PARAMETER(Exclusive);
			UNREFERENCED_PARAMETER(DeviceObject);
			DeviceObject = nullptr;
			return STATUS_NOT_IMPLEMENTED;
		}
#pragma warning(pop)
	}
}


#if 0
namespace wil {

#if defined(WIL_KERNEL_MODE) && (defined(_WDMDDK_) || defined(_NTDDK_)) && !defined(__WIL_RESOURCE_WDM)
#define __WIL_RESOURCE_WDM

	namespace details
	{
		struct kspin_lock_saved_irql
		{
			PKSPIN_LOCK spinLock = nullptr;
			KIRQL savedIrql = PASSIVE_LEVEL;

			kspin_lock_saved_irql() = default;

			kspin_lock_saved_irql(PKSPIN_LOCK /* spinLock */)
			{
				// This constructor exists simply to allow conversion of the pointer type to
				// pointer_storage type when constructing an invalid instance. The spinLock pointer
				// is expected to be nullptr.
			}

			// Exists to satisfy the interconvertibility requirement for pointer_storage and
			// pointer.
			explicit operator PKSPIN_LOCK() const
			{
				return spinLock;
			}

			_IRQL_requires_(DISPATCH_LEVEL)
				static
				void Release(_In_ _IRQL_restores_ const kspin_lock_saved_irql& spinLockSavedIrql)
			{
				KeReleaseSpinLock(spinLockSavedIrql.spinLock, spinLockSavedIrql.savedIrql);
			}
		};

		// On some architectures KeReleaseSpinLockFromDpcLevel is a macro, and we need a thunk
		// function we can take the address of.
		inline
			_IRQL_requires_min_(DISPATCH_LEVEL)
			void __stdcall ReleaseSpinLockFromDpcLevel(_Inout_ PKSPIN_LOCK spinLock) WI_NOEXCEPT
		{
			KeReleaseSpinLockFromDpcLevel(spinLock);
		}
	}

	using kspin_lock_guard = unique_any<PKSPIN_LOCK, decltype(details::kspin_lock_saved_irql::Release), & details::kspin_lock_saved_irql::Release,
		details::pointer_access_none, details::kspin_lock_saved_irql>;

	using kspin_lock_at_dpc_guard = unique_any<PKSPIN_LOCK, decltype(details::ReleaseSpinLockFromDpcLevel), & details::ReleaseSpinLockFromDpcLevel,
		details::pointer_access_none>;

	inline
		WI_NODISCARD
		_IRQL_requires_max_(DISPATCH_LEVEL)
		_IRQL_saves_
		_IRQL_raises_(DISPATCH_LEVEL)
		kspin_lock_guard
		acquire_kspin_lock(_In_ PKSPIN_LOCK spinLock)
	{
		details::kspin_lock_saved_irql spinLockSavedIrql;
		KeAcquireSpinLock(spinLock, &spinLockSavedIrql.savedIrql);
		spinLockSavedIrql.spinLock = spinLock;
		return kspin_lock_guard(spinLockSavedIrql);
	}

	inline
		WI_NODISCARD
		_IRQL_requires_min_(DISPATCH_LEVEL)
		kspin_lock_at_dpc_guard
		acquire_kspin_lock_at_dpc(_In_ PKSPIN_LOCK spinLock)
	{
		KeAcquireSpinLockAtDpcLevel(spinLock);
		return kspin_lock_at_dpc_guard(spinLock);
	}

	namespace details
	{
		// Define a templated type for pool functions in order to satisfy overload resolution below
		template <typename pointer, ULONG tag>
		struct pool_helpers
		{
			static inline
				_IRQL_requires_max_(DISPATCH_LEVEL)
				void __stdcall FreePoolWithTag(pointer value) WI_NOEXCEPT
			{
				if (value)
				{
					ExFreePoolWithTag(value, tag);
				}
			}
		};
	}

	template <typename pointer, ULONG tag = 0>
	using unique_tagged_pool_ptr = unique_any<pointer, decltype(details::pool_helpers<pointer, tag>::FreePoolWithTag), & details::pool_helpers<pointer, tag>::FreePoolWithTag>;

#endif // __WIL_RESOURCE_WDM

} // namespace wil

#endif // 0
