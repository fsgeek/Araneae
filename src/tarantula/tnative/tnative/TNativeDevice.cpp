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


NTSTATUS TNativeDevice::InvalidDeviceRequest(_In_ PIRP Irp) noexcept
{
	const NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}


_Use_decl_annotations_
NTSTATUS TNativeDevice::CompleteRequest(PIRP Irp, NTSTATUS Status) noexcept
{
	if (STATUS_PENDING != Status) {
		Irp->IoStatus.Status = Status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	}
	return Status;
}


_Use_decl_annotations_
NTSTATUS TNativeDevice::DispatchRequest(PDEVICE_OBJECT DeviceObject, PIRP Irp) noexcept
{
	TNativeDevice* nativeDevice = nullptr;
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION iosl = nullptr;

	while ((nullptr != DeviceObject) && (nullptr != Irp)) {
		nativeDevice = static_cast<TNativeDevice*>(DeviceObject->DeviceExtension);

		if (nullptr == nativeDevice) {
			break; // not valid
		}

		iosl = IoGetCurrentIrpStackLocation(Irp);

#pragma warning(push)
#pragma warning(disable: 26486) // nativeDevice is assumed to be valid; if not, we have big problems
		switch (iosl->MajorFunction) {
		case IRP_MJ_CREATE:
			status = nativeDevice->Create(Irp);
			break;
		case IRP_MJ_CREATE_NAMED_PIPE:
			status = nativeDevice->CreateNamedPipe(Irp);
			break;
		case IRP_MJ_CLOSE:
			status = nativeDevice->Close(Irp);
			break;
		case IRP_MJ_READ:
			status = nativeDevice->Read(Irp);
			break;
		case IRP_MJ_WRITE:
			status = nativeDevice->Write(Irp);
			break;
		case IRP_MJ_QUERY_INFORMATION:
			status = nativeDevice->QueryInformation(Irp);
			break;
		case IRP_MJ_SET_INFORMATION:
			status = nativeDevice->SetInformation(Irp);
			break;
		case IRP_MJ_QUERY_EA:
			status = nativeDevice->QueryEa(Irp);
			break;
		case IRP_MJ_SET_EA:
			status = nativeDevice->SetEa(Irp);
			break;
		case IRP_MJ_QUERY_VOLUME_INFORMATION:
			status = nativeDevice->QueryVolumeInformation(Irp);
			break;
		case IRP_MJ_SET_VOLUME_INFORMATION:
			status = nativeDevice->SetVolumeInformation(Irp);
			break;
		case IRP_MJ_DIRECTORY_CONTROL:
			status = nativeDevice->DirectoryControl(Irp);
			break;
		case IRP_MJ_FILE_SYSTEM_CONTROL:
			status = nativeDevice->FileSystemControl(Irp);
			break;
		case IRP_MJ_DEVICE_CONTROL:
			status = nativeDevice->DeviceControl(Irp);
		case IRP_MJ_INTERNAL_DEVICE_CONTROL:
			status = nativeDevice->InternalDeviceControl(Irp);
			break;
		case IRP_MJ_SHUTDOWN:
			status = nativeDevice->Shutdown(Irp);
			break;
		case IRP_MJ_LOCK_CONTROL:
			status = nativeDevice->LockControl(Irp);
			break;
		case IRP_MJ_CLEANUP:
			status = nativeDevice->Cleanup(Irp);
			break;
		case IRP_MJ_CREATE_MAILSLOT:
			status = nativeDevice->CreateMailslot(Irp);
			break;
		case IRP_MJ_QUERY_SECURITY:
			status = nativeDevice->QuerySecurity(Irp);
			break;
		case IRP_MJ_SET_SECURITY:
			status = nativeDevice->SetSecurity(Irp);
			break;
		case IRP_MJ_POWER:
			status = nativeDevice->Power(Irp);
			break;
		case IRP_MJ_SYSTEM_CONTROL:
			status = nativeDevice->SystemControl(Irp);
			break;
		case IRP_MJ_DEVICE_CHANGE:
			status = nativeDevice->DeviceChange(Irp);
			break;
		case IRP_MJ_QUERY_QUOTA:
			status = nativeDevice->QueryQuota(Irp);
			break;
		case IRP_MJ_SET_QUOTA:
			status = nativeDevice->SetQuota(Irp);
			break;
		case IRP_MJ_PNP:
			status = nativeDevice->PnP(Irp);
			break;
		default:
			status = nativeDevice->InvalidDeviceRequest(Irp);
			break;
		}
#pragma warning(pop)
	}

	return NTSTATUS();
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
			status = STATUS_INSUFFICIENT_RESOURCES;
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

void TNativeDevice::DeleteTNativeDevice(_In_ _Post_invalid_ TNativeDevice* NativeDevice) noexcept
{
	// Note: this also deletes the OBJECT
	if (nullptr != NativeDevice) {
		NativeDevice->DeleteDeviceObject();
	}
}
