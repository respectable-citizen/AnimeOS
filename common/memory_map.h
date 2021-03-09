#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <efi.h>

typedef struct {
	EFI_MEMORY_DESCRIPTOR *memory_descriptors;
	uint64_t memory_map_size;
	uint64_t map_key;
	uint64_t descriptor_size;
} MemoryMap;

#endif
