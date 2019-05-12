#include "TNativeDevice.h"

#pragma warning(disable:26409) // "don't use new/delete"
#pragma warning(disable:26400) // "don't use raw pointers from allocation"


#pragma warning(suppress:26493) // C style cast
static const ULONG TNativeDeviceMemTag = (ULONG)'eDsW';

_Use_decl_annotations_
void* TNativeDevice::operator new(size_t size)
{
	return ExAllocatePoolWithTag(PagedPool, size, TNativeDeviceMemTag);
}

_Use_decl_annotations_
void TNativeDevice::operator delete(void* p)
{
	if (nullptr != p) {
		ExFreePoolWithTag(p, TNativeDeviceMemTag);
	}
}


NTSTATUS TNativeDevice::InvalidDeviceRequest(_In_ PIRP Irp)
{
	const NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}


_Use_decl_annotations_
NTSTATUS TNativeDevice::CompleteRequest(PIRP Irp, NTSTATUS Status)
{
	if (STATUS_PENDING != Status) {
		Irp->IoStatus.Status = Status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	}
	return Status;
}

#pragma warning(push)
#pragma warning(disable:6014) // it insists the device object is being leaked, but we've saved it
#pragma warning(disable:26447) // we are in the kernel; new does not throw exceptions
_Must_inspect_result_
TNativeDevice* TNativeDevice::CreateTNativeDevice(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING DeviceName) noexcept
{
	PDEVICE_OBJECT deviceObject = nullptr;
	TNativeDevice* tnDevObj = nullptr;
	// auto tnDevObj = wil::make_unique_nothrow<DEVICE_OBJECT>(DriverObject, sizeof(TNativeDevice), DeviceName);
	NTSTATUS status = STATUS_INVALID_PARAMETER;

#pragma warning(suppress:6211) // we don't have an exception in this path, we won't leak the deviceObject
	status = IoCreateDevice(DriverObject, sizeof(TNativeDevice), DeviceName, FILE_DEVICE_FILE_SYSTEM, 0, FALSE, &deviceObject);

	while (NT_SUCCESS(status)) {

		if ((nullptr == deviceObject) || (nullptr == deviceObject->DeviceExtension)) {
			status = STATUS_NO_MEMORY;
			break;
		}

		tnDevObj = new (deviceObject->DeviceExtension) TNativeDevice;
		tnDevObj->m_DeviceObject = deviceObject;

		// Done
		break;
	}

	if (!NT_SUCCESS(status)) {
		// Error cleanup path
		if (nullptr != tnDevObj) {
			delete tnDevObj;
			tnDevObj = nullptr;
		}
	}

	return tnDevObj;
}
#pragma warning(pop)

#pragma warning(push)
#pragma disable(26432) // if you define any default operation you must define them all
TNativeDevice::~TNativeDevice() noexcept
{
	// TODO: need to delete the device object
	IoDeleteDevice(m_DeviceObject);
}

void TNativeDevice::DeleteTNativeDevice(_In_ _Post_invalid_ TNativeDevice* NativeDevice) noexcept
{
	delete NativeDevice;
}
#pragma warning(pop)
