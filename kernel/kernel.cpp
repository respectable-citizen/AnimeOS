#include <efi.h>

#include "../common/graphics_info.h"
#include "../common/memory_map.h"

#include "utils.hpp"

#include "memory_manager.hpp"

#include "text_renderer/text_renderer.hpp"

extern "C" void entry_point(MemoryMap memory_map, GraphicsInfo graphics_info, uint64_t kernel_start, uint64_t kernel_end) {
	asm("cli"); //Disable interrupts until we can handle them
				
	TextRenderer::initialise(graphics_info);
	TextRenderer::set_color(0xff0000);
	TextRenderer::draw_string((char* ) "AnimeRetard OS v1.0 booting\r\n\r\n");

	TextRenderer::set_color(0xffffff);
	TextRenderer::draw_string((char* ) "Initialising memory manager\r\n");
	MemoryManager::initialise(memory_map, kernel_start, kernel_end);
	
	TextRenderer::draw_string((char*) "test\r\n");
	
	/*
	int physical_page = MemoryManager::allocate_pages(1);
	int virtual_page = MemoryManager::allocate_pages(1);

	*((uint8_t*) MemoryManager::page_number_to_address(physical_page)) = 255;

	MemoryManager::setup_page_translation(physical_page, virtual_page, 1);
	TextRenderer::draw_string((char*) "test\r\n");
*/

	hang();
}
