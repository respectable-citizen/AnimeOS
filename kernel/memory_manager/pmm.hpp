#pragma once

#include "../common/memory_map.h"

#include "vmm.hpp"

#define PAGE_LOCKED 0
#define PAGE_FREE 1

namespace PMM {
	void initialise(MemoryMap, uint64_t, uint64_t);

	void lock_pages(uint64_t, uint64_t);
	void free_pages(uint64_t, uint64_t);
	uint64_t request_page();

	void* page_map();
	
	inline uint64_t address_to_page_number(void* address) {
		return (uint64_t) address / 4096;
	}
	
	inline void* page_number_to_address(uint64_t page_number) {
		return (void*) (page_number * 4096);
	}

	inline uint64_t bytes_to_pages(uint64_t bytes) {
		return (bytes / 4096) + 1;
	}

	inline uint64_t pages_to_bytes(uint64_t pages) {
		return 4096 * pages;
	}
};
