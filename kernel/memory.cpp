#include "memory.h"

extern "C" void* memcpy(void *destination, void *source, uint64_t size) {
	uint8_t *destination_bytes = (uint8_t*) destination;
	uint8_t *source_bytes = (uint8_t*) source;
	for (int i = 0; i < size; i++) *(destination_bytes + i) = *(source_bytes + i);
	return destination;
}
