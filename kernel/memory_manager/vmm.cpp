#include "vmm.hpp"

#include "pmm.hpp"

#include "text_renderer/text_renderer.hpp"
#include "lib/memory.hpp"

#include "utils.hpp"

namespace VMM {
	namespace {
		uint64_t *kernel_paging_table;
		uint64_t *current_paging_table;
	}
	
	void initialise(uint64_t kernel_page, uint64_t kernel_size_pages) {
		//Setup kernel page table
		kernel_paging_table = (uint64_t*) PMM::allocate_pages(1);
		memset((void*) kernel_paging_table, 0, 4096);
		UNUSED(kernel_page);
		UNUSED(kernel_size_pages);
		current_paging_table = kernel_paging_table;
		
		TextRenderer::draw_number((uint64_t) current_paging_table);

		//Identity map first 1MiB (it may be in use by firmware)
		//VMM::set_translation(0, 0, 256);
		
		//Identity map kernelspace
		VMM::set_translation(kernel_page, kernel_page, kernel_size_pages);

		//Also identity map the framebuffer
		uint64_t framebuffer_page = PMM::address_to_page_number(TextRenderer::graphics_info().address);
		uint64_t framebuffer_page_count = (TextRenderer::graphics_info().buffer_size / 4096) + 1;
		VMM::set_translation(framebuffer_page, framebuffer_page, framebuffer_page_count);
		
		asm("mov %0, %%cr3" : : "r" (current_paging_table)); //Now that we have setup the kernel VAS, we can safely enable paging without messing anything up
	}

	void set_translation(uint64_t physical_page, uint64_t virtual_page, uint64_t page_count, PageSize page_size) {
		if (page_size != PageSize::FOUR_KIB) TextRenderer::kernel_panic((char*) "Only 4KiB pages are currently supported.");

		for (uint64_t i = 0; i < page_count; i++)
			set_page_translation(physical_page + i, virtual_page + i, TABLE_ENTRY_PRESENT | TABLE_ENTRY_READWRITE, page_size);
	}
	
	void set_page_translation(uint64_t physical_page, uint64_t virtual_page, uint64_t flags, uint8_t page_size) {
		uint64_t *current_table = current_paging_table;
		for (uint8_t i = 0; i < page_size; i++) {
			uint64_t shift_amount = 9 * (3 - i);
			uint64_t mask = 0x1FFULL << shift_amount;
			uint64_t table_index = (virtual_page & mask) >> shift_amount;
	
			//We have reached the required table depth, time to create the actual table entry
			if (i == (page_size - 1)) {
				current_table[table_index] = (physical_page << 12) | flags;
				break;
			}

			void *subtable_address;
			if (current_table[table_index] & TABLE_ENTRY_PRESENT) {
				subtable_address = (void*) (((current_table[table_index]) >> 12) << 12); //We do this bitshifting to clear the flags and get just the address
			} else {
				//Table doesn't yet exist, allocate it
				void *sub_table = PMM::allocate_pages(1);
				memset(sub_table, 0, 4096);
				current_table[table_index] = (uint64_t) sub_table | flags;
				subtable_address = (uint64_t*) sub_table;
			}

			current_table = (uint64_t*) subtable_address;
		}
	}

}
