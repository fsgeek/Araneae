#include "TNControlDevice.h"
#include "TarantulaControl.h"


TNControlDevice::TNControlDevice() noexcept
{
}


#pragma warning(suppress:26436) // we have made this a protected non-virtual destructor
TNControlDevice::~TNControlDevice() noexcept
{
}

_Use_decl_annotations_
NTSTATUS TNControlDevice::Create(PIRP Irp) 
{
	PIO_STACK_LOCATION iosl = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while (nullptr != iosl) {

		if (iosl->FileObject->FileName.Length > 0) {
			// control object doesn't support naming
			status = STATUS_OBJECT_PATH_NOT_FOUND;
			iosl = nullptr;
			break;
		}

		status = STATUS_SUCCESS;
		break;
	}
	
	return status;
}

_Use_decl_annotations_
NTSTATUS TNControlDevice::Close(PIRP Irp)
{
	PIO_STACK_LOCATION iosl = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while (nullptr != iosl) {
		iosl = nullptr;
		status = STATUS_SUCCESS;
		break;
	}

	return status;
}

_Use_decl_annotations_
NTSTATUS TNControlDevice::DeviceControl(PIRP Irp) 
{
	PIO_STACK_LOCATION iosl = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while (nullptr != iosl) {
		const ULONG controlCode = iosl->Parameters.DeviceIoControl.IoControlCode;

		switch (controlCode) {
		default:
			status = STATUS_INVALID_PARAMETER;
			break;
		case TarantulaControlGetRequest:
			// TODO: implement this
			status = STATUS_NOT_IMPLEMENTED;
			break;
		}
		iosl = nullptr;
		break;
	}

	return status;
}

_Use_decl_annotations_
NTSTATUS TNControlDevice::Cleanup(PIRP Irp)
{
	PIO_STACK_LOCATION iosl = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while (nullptr != iosl) {
		iosl = nullptr;
		status = STATUS_SUCCESS;
		break;
	}

	return status;
}

#pragma warning(push)
#pragma warning(disable:6014) // it insists the device object is being leaked, but we've saved it
#pragma warning(disable:26447) // we are in the kernel; new does not throw exceptions
_Use_decl_annotations_
TNControlDevice* TNControlDevice::CreateTControlDevice(PDRIVER_OBJECT DriverObject, PUNICODE_STRING DeviceName) noexcept
{
	PDEVICE_OBJECT deviceObject = nullptr;
	TNControlDevice* tnDevObj = nullptr;
	// auto tnDevObj = wil::make_unique_nothrow<DEVICE_OBJECT>(DriverObject, sizeof(TNativeDevice), DeviceName);
	NTSTATUS status = STATUS_INVALID_PARAMETER;

#pragma warning(suppress:6211) // we don't have an exception in this path, we won't leak the deviceObject
	status = IoCreateDevice(DriverObject, sizeof(TNControlDevice), DeviceName, FILE_DEVICE_FILE_SYSTEM, 0, FALSE, &deviceObject);

	while (NT_SUCCESS(status)) {

		if ((nullptr == deviceObject) || (nullptr == deviceObject->DeviceExtension)) {
			status = STATUS_NO_MEMORY;
			break;
		}

#pragma warning(suppress:26493) // yes, it's a C-style cast
		tnDevObj = (TNControlDevice*)deviceObject;
		tnDevObj->SetDeviceObject(deviceObject);

		// Done
		break;
	}

	if (!NT_SUCCESS(status)) {
		// Error cleanup path
		if (nullptr != tnDevObj) {
			DeleteTNControlDevice(tnDevObj);
			tnDevObj = nullptr;
		}
	}

	return tnDevObj;
}

#pragma warning(pop)

_Use_decl_annotations_
void TNControlDevice::DeleteTNControlDevice(TNControlDevice* ControlDevice) noexcept
{
	// Note: this also deletes the OBJECT
	TNativeDevice::DeleteTNativeDevice(ControlDevice);
}
