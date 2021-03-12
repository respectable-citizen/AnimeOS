#include "memory_map.h"
#include "graphics_info.h"

typedef struct {
	MemoryMap memory_map;
	GraphicsInfo graphics_info;
	uint64_t kernel_start;
	uint64_t kernel_end;
} KernelArguments;
