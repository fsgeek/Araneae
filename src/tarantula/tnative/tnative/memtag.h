#pragma once

#include <ntdef.h>

union MemoryTag {
	UCHAR tag[4];
	ULONG tagvalue;
};

namespace Tarantula {
	const MemoryTag TNRegistryMemoryTag = { 'e','r','N','T' };
	const MemoryTag TNRegistryStringTag = { 's', 'r', 'N', 'T' };
	const MemoryTag TNRegistryValueEntryTag = { 'e','v','N','T' };
	const MemoryTag TNRegistryKeyEntryTag = { 'e','k', 'N','T' };
}
