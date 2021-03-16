#ifndef PMM_H
#define PMM_H

#include "../common/memory_map.h"

#include "vmm.hpp"

#define PAGE_LOCKED 0
#define PAGE_FREE 1

namespace PMM {
	void initialise(MemoryMap, uint64_t, uint64_t);

	uint8_t get_page(uint64_t);
	uint64_t set_page_status(uint64_t, uint64_t, uint8_t);
	void lock_pages(uint64_t, uint64_t);
	void free_pages(uint64_t, uint64_t);
	void* allocate_pages(uint64_t, bool, PageSize, bool);
	void* allocate_kernel_pages(uint64_t, PageSize = PageSize::FOUR_KIB);
	void* allocate_user_pages(uint64_t, PageSize = PageSize::FOUR_KIB);

	uint64_t address_to_page_number(void*);
	void* page_number_to_address(uint64_t);
	uint64_t bytes_to_pages(uint64_t);

	void* page_map();
	uint64_t page_map_size();
};

#endif
