#include <ntifs.h>
#include <ntstatus.h>
//#include "cpprt.h"
#include "tnative-resource.h"
#include "TNativeDevice.h"
#include "TNControlDevice.h"
#include "TNRegistry.h"
#include "cpprt.h"

static TNRegistry* TarantulaRegistry;
static TNRegistry* TarantulaRegistryParameters;
static TNControlDevice *TarantulaControlDevice;
static WCHAR TarantulaControlDeviceNameString[] = L"\\Tarantula";
#pragma warning(push)
#pragma warning(disable:26485) // yes, I know the string isn't a C++ string with array bounds
static UNICODE_STRING TNativeDeviceName = {
	sizeof(TarantulaControlDeviceNameString) - sizeof(WCHAR),
	sizeof(TarantulaControlDeviceNameString),
	TarantulaControlDeviceNameString
};
static WCHAR RegistryParametersNameString[] = L"Parameters";
static UNICODE_STRING RegistryParametersName = {
	sizeof(RegistryParametersNameString) - sizeof(WCHAR),
	sizeof(RegistryParametersNameString),
	RegistryParametersNameString
};
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:26461) // it can be marked const NOW, but that breaks when we use the input parameter
static DRIVER_UNLOAD TNativeUnload;
static void TNativeUnload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

	if (nullptr != TarantulaControlDevice) {
		TarantulaControlDevice->DeleteTNControlDevice(TarantulaControlDevice);
		TarantulaControlDevice = nullptr;
	}

	if (nullptr != TarantulaRegistryParameters) {
		TarantulaRegistryParameters->DeleteTNRegistry(TarantulaRegistryParameters);
		TarantulaRegistryParameters = nullptr;
	}

	if (nullptr != TarantulaRegistry) {
		TarantulaRegistry->DeleteTNRegistry(TarantulaRegistry);
		TarantulaRegistry = nullptr;
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

		TarantulaRegistry = TNRegistry::CreateTNRegistry(RegistryPath);
		if (nullptr == TarantulaRegistry) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		TarantulaRegistryParameters = TNRegistry::CreateTNRegistry(TarantulaRegistry, &RegistryParametersName);
		if (nullptr == TarantulaRegistryParameters) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		TarantulaControlDevice = TNControlDevice::CreateTNControlDevice(DriverObject, &TNativeDeviceName);
		if (nullptr == TarantulaControlDevice) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}
#pragma warning(push)
#pragma warning(disable: 28023 28169) // This is a generic handler, so I don't define it's dispatch type
#pragma warning(disable: 26446 26482) // yes, it's bad C++ code, but we're gluing worlds together
		for (unsigned index = 0; index < IRP_MJ_MAXIMUM_FUNCTION; index++) {
			DriverObject->MajorFunction[index] = TNativeDevice::DispatchRequest;
		}
#pragma warning(pop)

	}

	if (!NT_SUCCESS(status)) {
	
		// something went wrong, so we will go through the cleanup path.
		if (nullptr != DriverObject) {
			DriverObject->DriverUnload(DriverObject);
		}
	}

	return status;
}
