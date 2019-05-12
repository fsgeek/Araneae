#pragma once
#include "TNativeFileObject.h"

class TNativeFileContext
{
	struct FileContextData {
		ULONG s_Flags;
		TNativeFileObject* s_TNativeFileObject;
	};

	FileContextData m_FileContext;

	TNativeFileObject* m_TNativeFileObject;
	static __drv_allocatesMem(Mem) TNativeFileContext* allocate();
	static void deallocate(_In_ __drv_freesMem(Mem) TNativeFileContext* FileContext);
	TNativeFileContext();
	~TNativeFileContext();
public:
	_Must_inspect_result_ TNativeFileContext* CreateTNativeFileObject(_In_ TNativeFileObject* FileObject);
	void DeleteTNativeFileObject(_In_ _Post_ptr_invalid_ TNativeFileContext* FileContext);

};

