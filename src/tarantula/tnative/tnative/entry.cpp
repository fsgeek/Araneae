#include <ntifs.h>
#include <ntstatus.h>
//#include "cpprt.h"
#include "tnative-resource.h"
#include "TNativeDevice.h"
#include "cpprt.h"

static TNativeDevice *TNControlDevice;
static WCHAR TNativeDeviceNameString[] = L"\\Tarantula";
#pragma warning(push)
#pragma warning(disable:26485) // yes, I know the string isn't a C++ string with array bounds
static UNICODE_STRING TNativeDeviceName = {
	sizeof(TNativeDeviceNameString),
	sizeof(TNativeDeviceNameString) - sizeof(WCHAR),
	TNativeDeviceNameString
};
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:26461) // it can be marked const NOW, but that breaks when we use the input parameter
static DRIVER_UNLOAD TNativeUnload;
static void TNativeUnload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

	if (nullptr != TNControlDevice) {
		TNativeDevice::DeleteTNativeDevice(TNControlDevice);
		TNControlDevice = nullptr;
	}
	return;
}
#pragma warning(pop)

extern "C" DRIVER_INITIALIZE DriverEntry;
#pragma warning(suppress:26461)
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgBreakPoint(); // initial debug breakpoint

	status = cpp_rt_pre_init();
	if (!NT_SUCCESS(status)) {
		return status;
	}

	// I use a loop for clean exit control
	while (NT_SUCCESS(status)) {

		DriverObject->DriverUnload = TNativeUnload;

		// Done
		break;
	}

	status = cpp_rt_post_init(DriverObject, RegistryPath);

	while (NT_SUCCESS(status)) {
		TNControlDevice = TNativeDevice::CreateTNativeDevice(DriverObject, &TNativeDeviceName);
		if (nullptr == TNControlDevice) {
			status = STATUS_NO_MEMORY;
			break;
		}

	}

	if (!NT_SUCCESS(status)) {
	
		// something went wrong, so we will go through the cleanup path.
		if (nullptr != DriverObject) {
			DriverObject->DriverUnload(DriverObject);
		}
	}

	return status;
}
