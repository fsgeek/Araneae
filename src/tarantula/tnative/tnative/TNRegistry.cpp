#include "TNRegistry.h"
#include "TNativeLock.h"

static NTSTATUS CopyRegistryPath(_In_ PCUNICODE_STRING ExistingPath, _In_ PUNICODE_STRING NewPath, ULONG Tag) noexcept
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while ((nullptr != ExistingPath) && (nullptr != NewPath) && (nullptr != ExistingPath->Buffer)) {
		// save the path
		NewPath->Length = ExistingPath->Length;
		NewPath->MaximumLength = NewPath->Length + sizeof(WCHAR);
#pragma warning(suppress:26493) // C cast works here
		NewPath->Buffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, NewPath->MaximumLength, Tag);
		if (nullptr == NewPath->Buffer) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}
#pragma warning(suppress:26486) // the buffers have both been validated at this point
		RtlCopyMemory(NewPath->Buffer, ExistingPath->Buffer, NewPath->Length);
#pragma warning(suppress:26481) // not using span
		NewPath->Buffer[NewPath->Length / sizeof(WCHAR)] = '\0';

		status = STATUS_SUCCESS;
		break;
	}

	return status;
}

static HANDLE OpenRegistry(_In_opt_ HANDLE RegistryHandle, _In_ PUNICODE_STRING RegistryPath) noexcept
{
	HANDLE regHandle = nullptr;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while (nullptr != RegistryPath) {
		OBJECT_ATTRIBUTES oa = {
			sizeof(oa),
			RegistryHandle,
			RegistryPath,
			OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
			nullptr,
			nullptr
		};
		status = ZwOpenKey(&regHandle, KEY_READ, &oa);

		if (!NT_SUCCESS(status)) {
			regHandle = nullptr;
			break;
		}
		break;
	}

	return regHandle;
}

_Use_decl_annotations_
NTSTATUS TNRegistry::AddValue(const KEY_VALUE_FULL_INFORMATION &NewValue) noexcept
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	size_t size = 0;
	ValueEntry* ve = nullptr;
	ULONG_PTR ptr = 0;

	while (!NT_SUCCESS(status)) {

		// allocate enough space
		size = static_cast<size_t>(FIELD_OFFSET(ValueEntry, ValueInfo)) + FIELD_OFFSET(KEY_VALUE_FULL_INFORMATION, Name) + NewValue.DataOffset + NewValue.DataLength;
		ve = static_cast<ValueEntry*>(ExAllocatePoolWithTag(PagedPool, size, Tarantula::TNRegistryValueEntryTag.tagvalue));
		if (nullptr == ve) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		// set up the new entry
#pragma warning(suppress:26490) // it's this or a C-cast
		ptr = (reinterpret_cast<ULONG_PTR>(ve)) + FIELD_OFFSET(ValueEntry, ValueInfo);
		RtlCopyMemory(reinterpret_cast<void *>(ptr), &NewValue, size - FIELD_OFFSET(ValueEntry, ValueInfo));
#pragma warning(suppress:26485) // I know there's no array bounds here
		RtlInitUnicodeString(&ve->Name, ve->ValueInfo.Name);

		// Add it to the list
		InsertTailList(&m_ValueList, &ve->ListEntry);
	}
	
	return status;
}

_Use_decl_annotations_
NTSTATUS TNRegistry::AddKey(const KEY_BASIC_INFORMATION & NewKey) noexcept
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	size_t size = 0;
	ValueEntry* ke = nullptr;
	ULONG_PTR ptr = 0;

	while (!NT_SUCCESS(status)) {

		// allocate enough space
		size = (static_cast<size_t>(FIELD_OFFSET(ValueEntry, KeyInfo))) + FIELD_OFFSET(KEY_BASIC_INFORMATION, Name) + NewKey.NameLength;
		ke = static_cast<ValueEntry*>(ExAllocatePoolWithTag(PagedPool, size, Tarantula::TNRegistryKeyEntryTag.tagvalue));
		if (nullptr == ke) {
			status = STATUS_INSUFFICIENT_RESOURCES;
			break;
		}

		// set up the new entry
#pragma warning(suppress:26490) // it's this or a C-cast
		ptr = reinterpret_cast<ULONG_PTR>(ke) + FIELD_OFFSET(ValueEntry, KeyInfo);
		RtlCopyMemory(reinterpret_cast<void*>(ptr), &NewKey, size - FIELD_OFFSET(ValueEntry, KeyInfo));
#pragma warning(suppress:26485) // yes it is an unbound array
		RtlInitUnicodeString(&ke->Name, ke->KeyInfo.Name);

		// Add it to the list
		InsertTailList(&m_KeyList, &ke->ListEntry);
	}

	return status;
}

void TNRegistry::FreeValueList(void) noexcept
{
	ValueEntry* ve = nullptr;
	LIST_ENTRY* le = nullptr;

	while (!IsListEmpty(&m_ValueList)) {
		le = RemoveHeadList(&m_ValueList);
#pragma warning(suppress:26481) // I can't use span
		ve = CONTAINING_RECORD(le, ValueEntry, ListEntry);
		ExFreePoolWithTag(ve, Tarantula::TNRegistryValueEntryTag.tagvalue);
	}
}

void TNRegistry::FreeKeyList(void) noexcept
{
	return;
}

_Use_decl_annotations_
NTSTATUS TNRegistry::LoadRegistryValues() noexcept
{
	return NTSTATUS();
}

_Use_decl_annotations_
TNRegistry* TNRegistry::CreateTNRegistry(PCUNICODE_STRING RegistryPath) noexcept
{
#pragma warning(suppress:6014 26400 26409) // in kernel, this is how we allocate memory.
	TNRegistry* registry = new TNRegistry;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while (nullptr != registry) {
		registry->m_RegistryPath.Length = 0;
		registry->m_RegistryPath.Buffer = nullptr;
		registry->m_RegistryHandle = nullptr;

		if (nullptr == RegistryPath) {
			// nothing else we can do in this case for initialization
			delete registry;
			break;
		}
		// save the path
		status = CopyRegistryPath(RegistryPath, &registry->m_RegistryPath, Tarantula::TNRegistryStringTag.tagvalue);
		if (!NT_SUCCESS(status)) {
			delete registry;
			registry = nullptr;
			break;
		}

		registry->m_RegistryHandle = OpenRegistry(nullptr, &registry->m_RegistryPath);
		if (nullptr == registry->m_RegistryHandle) {
			DeleteTNRegistry(registry);
			registry = nullptr;
			break;
		}

		break;
	}

#pragma warning(suppress:6001) // it IS initialized
	return registry;
}


#pragma warning(push)
#pragma warning(disable:6014)
#pragma warning(disable:26400 26447 26409) // we're in the kernel, this is how we allocate memory
_Use_decl_annotations_
TNRegistry* TNRegistry::CreateTNRegistry(TNRegistry *Registry, PCUNICODE_STRING RegistryPath) noexcept
{
	TNRegistry* registry = nullptr;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	while ((nullptr != Registry) && (nullptr != RegistryPath)) {
		registry = new TNRegistry;

		if (nullptr == registry) {
			break;
		}

		registry->m_RegistryPath.Length = 0;
		registry->m_RegistryPath.Buffer = nullptr;
		registry->m_RegistryHandle = nullptr;

		if (nullptr == RegistryPath) {
			// nothing else we can do in this case for initialization
			delete registry;
			break;
		}

		// save the path
		status = CopyRegistryPath(RegistryPath, &registry->m_RegistryPath, Tarantula::TNRegistryStringTag.tagvalue);
		if (!NT_SUCCESS(status)) {
			delete registry;
			registry = nullptr;
			break;
		}

		// open registry relative to handle 
		registry->m_RegistryHandle = OpenRegistry(Registry->GetRegistryHandle(), &registry->m_RegistryPath);

		if (nullptr == registry->m_RegistryHandle) {
			DeleteTNRegistry(registry);
			registry = nullptr;
			break;
		}

		break;
	}

	return registry;
}
#pragma warning(pop)

_Use_decl_annotations_
void TNRegistry::DeleteTNRegistry(TNRegistry* Registry) noexcept
{
	while (nullptr != Registry) {
		if (nullptr != Registry->m_RegistryPath.Buffer) {
			ExFreePoolWithTag(Registry->m_RegistryPath.Buffer, Tarantula::TNRegistryStringTag.tagvalue);
			Registry->m_RegistryPath.Buffer = nullptr;
		}

		if (nullptr != Registry->m_RegistryHandle) {
#pragma warning(suppress:26493) // how else do I note that I'm not looking at the return code besides a cast?
			(void) ZwClose(Registry->m_RegistryHandle);
			Registry->m_RegistryHandle = nullptr;
		}

		break;
	}
}

_Use_decl_annotations_
NTSTATUS TNRegistry::ReadDwordValue(PCUNICODE_STRING ValueName, ULONG& Value)
{
#pragma warning(suppress:26496) // we'll use it more...
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	UNREFERENCED_PARAMETER(ValueName);
	Value = 0;
#if 0
	union {
		ULONGLONG padding[8];
		KEY_VALUE_BASIC_INFORMATION basicinfo;
		KEY_VALUE_FULL_INFORMATION fullinfo;
		UCHAR data[1];
	} regvalue = { 0 };
	ULONG outlength;
	
	if (nullptr == m_RegistryHandle) {
		return STATUS_INVALID_HANDLE;
	}

	status = ZwQueryValueKey(m_RegistryHandle, ValueName, KeyValueFullInformation, &regvalue, sizeof(regvalue), &outlength);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	// check bounds
	if (regvalue.fullinfo.DataOffset + )

	RtlCopyMemory(&Value, &regvalue.data[regvalue.fullinfo.DataOffset], regvalue.fullinfo.DataLength);
#endif // 0
	return status;
}

#if 0
NTSTATUS TNRegistry::ReadStringValue(UNICODE_STRING& Value)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	return status
}
#endif // 0
