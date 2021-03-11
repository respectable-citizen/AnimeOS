#include "gdt.hpp"

#include "../lib/memory.hpp"

#include "../text_renderer/text_renderer.hpp"

namespace GDT {
	namespace {
		GDTEntry m_GDT[3] __attribute__((aligned (4096)));
	}	
				
	void set_gdt_entry(uint16_t index, bool is_executable) {
		memset(&m_GDT[index], 0, sizeof(m_GDT[index]));
		m_GDT[index].type = GDT_ENTRY_TYPE_CODE_OR_DATA;
		m_GDT[index].present = 1;
		m_GDT[index].read_write = 1;
		m_GDT[index].executable = is_executable;
		m_GDT[index].is_64_bit = is_executable; //Don't set to 1 if not executable as this bit is reserved for data segments
	}

	void initialise() {
		memset(&m_GDT[0], 0, sizeof(m_GDT[0])); //Null segment
		set_gdt_entry(1, true);                 //Code segment
		set_gdt_entry(2, false);                //Data segment

		//Setup GDTR
		GDTR gdt_descriptor;
		gdt_descriptor.size = sizeof(m_GDT) - 1; //Size - 1 as per spec
		gdt_descriptor.address = (uint64_t) &m_GDT[0];

		load_gdt(&gdt_descriptor);
	}
}
