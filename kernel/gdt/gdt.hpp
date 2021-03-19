#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_ENTRY_TYPE_SYSTEM 0
#define GDT_ENTRY_TYPE_CODE_OR_DATA 1

//Holds the address and size of the GDT, this is what we provide to the lgdt instruction
struct GDTR {
	uint16_t size;
	uint64_t address;
} __attribute((packed));

//Each entry describes some amount of memory
struct GDTEntry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t  base_middle;
	uint8_t  accessed:1;
	uint8_t  read_write:1;
	uint8_t  direction_conforming:1;
	uint8_t  executable:1;
	uint8_t  type:1;
	uint8_t  ring:2;
	uint8_t  present:1;
	uint8_t  limit_high:4;
	uint8_t  available:1;
	uint8_t  is_64_bit:1;
	uint8_t  is_32_bit:1;
	uint8_t  granularity:1;
	uint8_t  base_high;
} __attribute__((packed));

/*
struct TSS {
	uint32_t zero_0;
	uint32_t rsp0_low;
	uint32_t rsp0_high;
	uint32_t rsp1_low;
	uint32_t rsp1_high;
	uint32_t rsp2_low;
	uint32_t rsp2_high;

	uint64_t reserved1;
	uint32_t ist1_low;
	uint32_t ist1_high;
	uint32_t ist2_low;
	uint32_t ist2_high;
	uint32_t ist3_low;
	uint32_t ist3_high;
	uint32_t ist4_low;
	uint32_t ist4_high;
	uint32_t ist5_low;
	uint32_t ist5_high;
	uint32_t ist6_low;
	uint32_t ist6_high;
	uint32_t ist7_low;
	uint32_t ist7_high;

	uint64_t zero_2;
	uint16_t zero_3;
	uint16_t iopb_offset;
} __attribute__((packed));
*/

namespace GDT {
	void set_gdt_entry(uint16_t, bool);
	void initialise();
	extern "C" void load_gdt(GDTR*);
}

#endif
