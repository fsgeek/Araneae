#include "TNativeFileContext.h"

#pragma warning(suppress:26493)
static const ULONG TNativeFileContextTag = (ULONG)'cfNT';


_Use_decl_annotations_
TNativeFileContext* TNativeFileContext::CreateTNativeFileObject(TNativeFileObject* FileObject)
{
	TNativeFileContext* fileContext = TNativeFileContext::allocate();

	while (nullptr != fileContext) {
		fileContext->m_FileContext.s_TNativeFileObject = FileObject;
	}

	return fileContext;

}

_Use_decl_annotations_
TNativeFileContext* TNativeFileContext::allocate(void)
{
#pragma warning(suppress:26493)
	TNativeFileContext *fileContext = (TNativeFileContext*)ExAllocatePoolWithTag(PagedPool, sizeof(TNativeFileContext), TNativeFileContextTag);

	if (nullptr != fileContext) {
		RtlSecureZeroMemory(fileContext, sizeof(TNativeFileContext));
	}

	return fileContext;
}

_Use_decl_annotations_
void TNativeFileContext::deallocate(TNativeFileContext* FileContext)
{
	if (nullptr != FileContext) {
		RtlSecureZeroMemory(FileContext, sizeof(TNativeFileContext));
		ExFreePoolWithTag(FileContext, TNativeFileContextTag);
	}
}
