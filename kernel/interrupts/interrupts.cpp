#include "interrupts.hpp"

#include "lib/memory.hpp"

#include "text_renderer/text_renderer.hpp"

#include "isr.hpp"

namespace Interrupts {
	namespace {
		IDTEntry m_IDT[256] __attribute__((aligned (4096)));
	}

	IDTEntry* idt() {
		return m_IDT;
	}
	
	void enable() {
		asm("sti");
	}

	void disable() {
		asm("cli");
	}

	void set_idt_entry(uint8_t index, uint64_t isr_address, uint16_t selector, uint8_t gate_type) {
		m_IDT[index].offset_low = isr_address & 0xFFFF;
		m_IDT[index].offset_middle = (isr_address >> 16) & 0xFFFF;
		m_IDT[index].offset_high = (isr_address >> 32) & 0xFFFFFFFF;
		m_IDT[index].selector = selector;
		m_IDT[index].gate_type = gate_type;
		m_IDT[index].present = 1;
	}

	void initialise() {
		memset(m_IDT, 0, sizeof(IDTEntry) * 256);

		set_idt_entry(8, (uint64_t) &isr8, 0x8, GATE_TYPE_TRAP); //Double fault handler
		set_idt_entry(14, (uint64_t) &isr14, 0x8, GATE_TYPE_TRAP); //Page fault handler
		set_idt_entry(0x80, (uint64_t) &isr128, 0x8, GATE_TYPE_TRAP); //SYSCALL handler

		IDTR idtr;
		idtr.size = sizeof(m_IDT) - 1;
		idtr.address = (uint64_t) m_IDT;

		//Load IDT and re-enable interrupts because we can now handle them
		asm("lidt %0" : : "m" (idtr));
		Interrupts::enable();
	}
}
