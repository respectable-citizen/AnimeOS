#include <efi.h>

#include "../common/kernel_arguments.h"

#include "utils.hpp"

#include "lib/memory.hpp"

#include "gdt/gdt.hpp"

#include "interrupts/interrupts.hpp"

#include "memory_manager/pmm.hpp"
#include "memory_manager/vmm.hpp"
#include "memory_manager/heap.hpp"

#include "text_renderer/text_renderer.hpp"

extern "C" void kernel_main() {
	Interrupts::disable(); //Disable interrupts until we can handle them
	
	//Read kernel arguments out of RDI register and decompose the struct into individual variables
	KernelArguments *kernel_arguments;
	asm volatile ("mov %%rdi, %0": "=g"(kernel_arguments)::"memory");

	MemoryMap memory_map = kernel_arguments->memory_map;
	GraphicsInfo graphics_info = kernel_arguments->graphics_info;
	uint64_t kernel_start = kernel_arguments->kernel_start;
	uint64_t kernel_end  = kernel_arguments->kernel_end;

	//Initialise kernel
	TextRenderer::initialise(graphics_info);
	TextRenderer::set_color(0xff0000);
	TextRenderer::draw_string((char* ) "AnimeOS booting\r\n\r\n");
	TextRenderer::set_color(0xffffff);
		
	TextRenderer::draw_string((char* ) "Initialising GDT\r\n");
	GDT::initialise();
	
	TextRenderer::draw_string((char* ) "Initialising IDT\r\n");
	Interrupts::initialise();

	TextRenderer::draw_string((char* ) "Initialising memory manager\r\n");

	uint64_t kernel_page = PMM::address_to_page_number((void*) kernel_start);
	uint64_t kernel_end_page = PMM::address_to_page_number((void*) kernel_end);
	uint64_t kernel_size_pages = kernel_end_page - kernel_page + 1;

	PMM::initialise(memory_map, kernel_page, kernel_size_pages);
	VMM::initialise(kernel_page, kernel_size_pages);
	
	asm("int $0x0e");
	hang();



	//Heap::initialise(16); //Let's start with 64KiB for kernel heap
	//for (;;) Heap::malloc(10);

	TextRenderer::draw_string((char* ) "it all worked");

	hang();
}
