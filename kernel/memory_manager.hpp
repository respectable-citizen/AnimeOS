#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "../common/memory_map.h"

#define PAGE_LOCKED 0x0
#define PAGE_RESERVED 0x1
#define PAGE_FREE 0x2

namespace MemoryManager {
	void initialise(MemoryMap, uint64_t, uint64_t);
	void setup_kernel_virtual_address_space(uint64_t, uint64_t);

	//PMM
	uint64_t free_pages();
	uint64_t reserved_pages();
	uint64_t used_pages();
	uint64_t total_pages();
	
	uint8_t get_page(uint64_t);
	uint64_t find_contiguous_pages(uint64_t);
	uint64_t modify_pages(uint64_t, uint64_t, uint8_t);
	void lock_pages(uint64_t, uint64_t);
	void free_pages(uint64_t, uint64_t);
	void reserve_pages(uint64_t, uint64_t);
	void unreserve_pages(uint64_t, uint64_t);
	uint64_t allocate_pages(uint64_t, bool = false);
	
	uint64_t address_to_page_number(void*);
	void* page_number_to_address(uint64_t);
	
	//VMM
	void setup_page_translation(uint64_t, uint64_t, uint64_t);
};

#endif
