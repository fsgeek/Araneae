#pragma once
#include <ntifs.h>
#include "TNativeDevice.h"

class TNControlDevice :	public TNativeDevice
{
	TNControlDevice() noexcept;

protected:
	~TNControlDevice() noexcept;

public:
	NTSTATUS Create(_In_ PIRP Irp) override;
	NTSTATUS Close(_In_ PIRP Irp) override;
	NTSTATUS DeviceControl(_In_ PIRP Irp) override;
	NTSTATUS Cleanup(_In_ PIRP Irp) override;

	_Must_inspect_result_
	static TNControlDevice* CreateTControlDevice(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING DeviceName) noexcept;
	static void DeleteTNControlDevice(_In_ _Post_invalid_ TNControlDevice* ControlDevice) noexcept;
	// we don't implement these, so we delete the default implementations.
	TNControlDevice(const TNControlDevice&) = delete;
	TNControlDevice& operator=(const TNControlDevice& TNDevice) = delete;
	TNControlDevice(TNControlDevice&&) = delete;
	TNControlDevice& operator=(TNControlDevice&&) = delete;
};

