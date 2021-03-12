#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define GATE_TYPE_TRAP 0b1111
#define GATE_TYPE_INTERRUPT 0b1110

struct IDTEntry {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t ist_index:3;
	uint8_t zero_1:5;
	uint8_t gate_type:4;
	uint8_t storage_segment:1;
	uint8_t descriptor_privilege_level:2;
	uint8_t present:1;
	uint16_t offset_middle;
	uint32_t offset_high;
	uint32_t zero_2;
} __attribute((packed));

struct IDTR {
	uint16_t size;
	uint64_t address;
} __attribute((packed));

namespace IDT {
	IDTEntry* idt();
	void set_idt_entry(uint8_t, uint64_t, uint16_t, uint8_t);
	void initialise();
}

#endif
