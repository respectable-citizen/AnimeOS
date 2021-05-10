#include "memory.hpp"

#include "../memory_manager/pmm.hpp"

extern "C" void* memcpy(void *destination, void *source, uint64_t size) {
	uint8_t *destination_bytes = (uint8_t*) destination;
	uint8_t *source_bytes = (uint8_t*) source;
	for (uint64_t i = 0; i < size; i++) *(destination_bytes + i) = *(source_bytes + i);
	return destination;
}
extern "C" void* memmove(void *destination, void *source, uint64_t size) {
	uint64_t page_count = PMM::bytes_to_pages(size);
	void *temporary_memory = PMM::allocate_kernel_pages(page_count);
	memcpy(temporary_memory, source, size);
	memcpy(destination, temporary_memory, size);
	PMM::free_pages(PMM::address_to_page_number(temporary_memory), page_count);
	return destination;
}


extern "C" void* memset(void *location, uint8_t byte, uint64_t size) {
	uint8_t *location_bytes = (uint8_t*) location;
	for (uint64_t i = 0; i < size; i++) *(location_bytes + i) = byte;
	return location;
}
