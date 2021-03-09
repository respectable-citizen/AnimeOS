#include "memory.hpp"

extern "C" void* memcpy(void *destination, void *source, uint64_t size) {
	uint8_t *destination_bytes = (uint8_t*) destination;
	uint8_t *source_bytes = (uint8_t*) source;
	for (uint64_t i = 0; i < size; i++) *(destination_bytes + i) = *(source_bytes + i);
	return destination;
}

extern "C" void* memset(void *location, uint8_t byte, uint64_t size) {
	uint8_t *location_bytes = (uint8_t*) location;
	for (uint64_t i = 0; i < size; i++) *(location_bytes + i) = byte;
	return location;
}
