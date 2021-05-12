#pragma once

#include <stdint.h>

#define KERNEL_MEMORY_PAGES 524288 //The first KERNEL_MEMORY_PAGES of memory are identity mapped and used solely for the kernel

namespace VMM {
	void initialise(uint64_t, uint64_t);
	uint64_t create_table_entry(uint64_t, uint64_t, bool);
	void set_translation(uint64_t, uint64_t, uint64_t, uint64_t = 1);
	void set_page_translation(uint64_t, uint64_t, uint64_t);
};
