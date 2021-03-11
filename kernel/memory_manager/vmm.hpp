#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define TABLE_ENTRY_PRESENT 0b1
#define TABLE_ENTRY_READWRITE 0b10

namespace VMM {
	void initialise(uint64_t, uint64_t);
	void separate_table_indexes(uint64_t, uint16_t*, uint16_t*, uint16_t*, uint16_t*);
	uint64_t create_table_entry(uint64_t, uint64_t);
	void set_page_translation(uint64_t, uint64_t, uint64_t);
	void recursively_set_table_translation(uint64_t*, uint64_t, uint64_t, uint8_t, uint64_t);
};

#endif
