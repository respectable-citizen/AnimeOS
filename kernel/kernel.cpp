#include <efi.h>

#include "../common/memory_map.h"

extern "C" uint64_t entry_point(MemoryMap memory_map) {
	return memory_map.map_key;
}
