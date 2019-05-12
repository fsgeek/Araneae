#pragma once
#include <ntifs.h>

class TNativeFileObject
{
	struct FileContextNonPaged {
		FSRTL_ADVANCED_FCB_HEADER header;
		SECTION_OBJECT_POINTERS sop;
	};

	struct FileContextPaged {

	};

	FileContextNonPaged *m_FileContextNonPaged;

	TNativeFileObject();
	virtual ~TNativeFileObject();
public:
	static TNativeFileObject* CreateTNativeFileObject(_In_ PFILE_OBJECT FileObject);
};

