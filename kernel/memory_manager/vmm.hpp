#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define TABLE_ENTRY_PRESENT 0b1
#define TABLE_ENTRY_READWRITE 0b10

#define KERNEL_MEMORY_PAGES 524288 //The first KERNEL_MEMORY_PAGES of memory are identity mapped and used solely for the kernel

enum PageSize {
	FOUR_KIB = 4,
	TWO_MIB = 3,
	ONE_GIB = 2
};

namespace VMM {
	void initialise(uint64_t, uint64_t);
	uint64_t create_table_entry(uint64_t, uint64_t, bool);
	void set_translation(uint64_t, uint64_t, uint64_t, PageSize = FOUR_KIB);
	void set_page_translation(uint64_t, uint64_t, uint64_t, uint8_t);
};

#endif
