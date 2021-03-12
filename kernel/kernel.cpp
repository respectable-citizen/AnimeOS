#include <efi.h>

#include "../common/graphics_info.h"
#include "../common/memory_map.h"

#include "utils.hpp"

#include "lib/memory.hpp"

#include "gdt/gdt.hpp"

#include "interrupts/idt.hpp"

#include "memory_manager/vmm.hpp"
#include "memory_manager/pmm.hpp"

#include "text_renderer/text_renderer.hpp"

extern "C" void kernel_main(MemoryMap memory_map, GraphicsInfo graphics_info, uint64_t kernel_start, uint64_t kernel_end) {
	asm("cli"); //Disable interrupts until we can handle them
				
	TextRenderer::initialise(graphics_info);
	TextRenderer::set_color(0xff0000);
	TextRenderer::draw_string((char* ) "AnimeOS booting\r\n\r\n");
	TextRenderer::set_color(0xffffff);
		
	TextRenderer::draw_string((char* ) "Initialising GDT\r\n");
	GDT::initialise();
	
	TextRenderer::draw_string((char* ) "Initialising IDT\r\n");
	IDT::initialise();
	TextRenderer::draw_string((char* ) "idt: \r\n");
	TextRenderer::draw_number((uint64_t) IDT::idt());
	TextRenderer::draw_string((char* ) "\r\n");
	
	TextRenderer::draw_string((char* ) "Initialising memory manager\r\n");

	uint64_t kernel_page = PMM::address_to_page_number((void*) kernel_start);
	uint64_t kernel_page_end = PMM::address_to_page_number((void*) kernel_end) + 1;
	uint64_t kernel_size_pages = kernel_page_end - kernel_page;
	
	PMM::initialise(memory_map, kernel_page, kernel_size_pages);
	VMM::initialise(kernel_page, kernel_size_pages);
	for (;;);

	TextRenderer::kernel_panic((char*) "ladies and gentleman we got em");

	hang();
}
