#include "TNRegistry.h"

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
		status = CopyRegistryPath(RegistryPath, &registry->m_RegistryPath, DefaultStringTag.tagvalue);
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
		status = CopyRegistryPath(RegistryPath, &registry->m_RegistryPath, DefaultStringTag.tagvalue);
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
			ExFreePoolWithTag(Registry->m_RegistryPath.Buffer, DefaultStringTag.tagvalue);
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

const MemoryTag TNRegistry::DefaultMemoryTag = { 'e','r','N','T' };
const MemoryTag TNRegistry::DefaultStringTag = { 's', 'r', 'N', 'T' };
