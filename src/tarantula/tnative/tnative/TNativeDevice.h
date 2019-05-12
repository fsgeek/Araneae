
#pragma once
#include <wdm.h>

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

class TNativeDevice
{

	NTSTATUS InvalidDeviceRequest(_In_ PIRP Irp);
	PDEVICE_OBJECT m_DeviceObject = nullptr;
	void* operator new(size_t size, void* memory) { UNREFERENCED_PARAMETER(size); return memory; }
	__drv_allocatesMem(Mem)
	void* operator new(_In_ size_t size);
	void operator delete(_In_ _Post_invalid_ void* memory);

protected:
	void SetDeviceObject(_In_ PDEVICE_OBJECT DeviceObject) noexcept { m_DeviceObject = DeviceObject; }
	void DeleteDeviceObject(void) noexcept { if (nullptr != m_DeviceObject) IoDeleteDevice(m_DeviceObject); m_DeviceObject = nullptr; }

public:
	virtual NTSTATUS Create(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS CreateNamedPipe(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS Close(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS Read(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS Write(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS QueryInformation(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS SetInformation(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS QueryEa(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS SetEa(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS QueryVolumeInformation(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS SetVolumeInformation(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS DirectoryControl(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS FileSystemControl(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS DeviceControl(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS InternalDeviceControl(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS Shutdown(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS LockControl(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS Cleanup(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS CreateMailslot(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS QuerySecurity(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS SetSecurity(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS Power(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS SystemControl(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS DeviceChange(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS QueryQuota(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS SetQuota(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS PnP(_In_ PIRP Irp) { return InvalidDeviceRequest(Irp); }
	virtual NTSTATUS CompleteRequest(_In_ _Post_invalid_ PIRP Irp, NTSTATUS Status);
	virtual ~TNativeDevice() noexcept { /* object lives inside the device extension */};

	PDEVICE_OBJECT GetDeviceObject(void) noexcept { return m_DeviceObject; }

	_Must_inspect_result_
	static TNativeDevice* CreateTNativeDevice(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING DeviceName) noexcept;
	static void DeleteTNativeDevice(_In_ _Post_invalid_ TNativeDevice *NativeDevice) noexcept;
	TNativeDevice() noexcept {}; // since we're using the placement new to stick this in the device extension, there's no work required.
	// we don't implement these, so we delete the default implementations.
	TNativeDevice(const TNativeDevice&) = delete;
	TNativeDevice& operator=(const TNativeDevice& TNDevice) = delete;
	TNativeDevice(TNativeDevice&&) = delete;
	TNativeDevice& operator=(TNativeDevice&&) = delete;
};
