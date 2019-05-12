#pragma once
#include <ntifs.h>

class TNativeLock
{
	TNativeLock();
	~TNativeLock();
	ERESOURCE m_resource;
	TNativeLock* operator new() { ExAllocatePoolWithTag() }
public:
	static TNativeLock* CreateTNativeLock() {
		
		ExInitializeResource(&m_resource);
	}

	PERESOURCE GetLock() { return &m_resource; }

	static void DeleteTNativeLock(TNativeLock* Lock) {
		ExDeleteResourceLite(&m_resource);
	};
};



TNativeLock::TNativeLock()
{
}


TNativeLock::~TNativeLock()
{
}
