#include <efi.h>

#include "../common/kernel_arguments.h"

#include "utils.hpp"

#include "lib/memory.hpp"

#include "gdt/gdt.hpp"

#include "interrupts/interrupts.hpp"

#include "memory_manager/pmm.hpp"
#include "memory_manager/vmm.hpp"
#include "memory_manager/block32.hpp"
#include "memory_manager/heap.hpp"

#include "cpu/cpu.hpp"

#include "text_renderer/text_renderer.hpp"

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void call_global_constructors() {
	for (constructor* i = &start_ctors; i != &end_ctors; i++) (*i)();
}

extern "C" void kernel_main() {
	Interrupts::disable(); //Disable interrupts until we can handle them
	
	//Read kernel arguments out of RDI register and decompose the struct into individual variables
	KernelArguments *kernel_arguments;
	asm("mov %%rdi, %0": "=g"(kernel_arguments)::"memory");

	MemoryMap memory_map = kernel_arguments->memory_map;
	GraphicsInfo graphics_info = kernel_arguments->graphics_info;
	void *kernel_start = (void*) kernel_arguments->kernel_start;
	void *kernel_end = (void*) kernel_arguments->kernel_end;

	//Initialise kernel
	TextRenderer::initialise(graphics_info);
	TextRenderer::set_color(0xff0000);
	TextRenderer::draw_string((char*) "AnimeOS booting\r\n\r\n");
	TextRenderer::set_color(0xffffff);

	//handle_constructors();
	TextRenderer::draw_string((char*) "Initialising GDT\r\n");
	GDT::initialise();
	
	TextRenderer::draw_string((char*) "Initialising IDT\r\n");
	Interrupts::initialise();
	
	TextRenderer::draw_string((char*) "Initialising CPU\r\n");
	CPU::initialise(); //From this point on we can use advanced CPU features
	
	TextRenderer::draw_string((char*) "Initialising memory manager\r\n");

	uint64_t kernel_page = PMM::address_to_page_number(kernel_start);
	uint64_t kernel_end_page = PMM::address_to_page_number(kernel_end);
	uint64_t kernel_size_pages = kernel_end_page - kernel_page + 1;

	PMM::initialise(memory_map, kernel_page, kernel_size_pages);
	VMM::initialise(kernel_start, kernel_end);

	//call_global_constructors(); //This would usually be done before the kernel main function is even called, but some of our global constructors rely on the memory manager and such we have to wait until they are initialised before we call the global constructors
	
	//Block32::initialise();
	//Heap::initialise();

	//for (uint64_t i = 0; i < 1000000000; i++) {};
	TextRenderer::set_color(0x0000ff);
	TextRenderer::fill_screen();
	TextRenderer::draw_string((char*) "AnimeOS is operational! :)\r\n");
	hang();
}
