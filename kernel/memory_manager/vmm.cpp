#include "vmm.hpp"

#include "pmm.hpp"

#include "text_renderer/text_renderer.hpp"
#include "lib/memory.hpp"

namespace VMM {
	namespace {
		uint64_t *kernel_paging_table;
		uint64_t *current_paging_table;
	}
	
	void initialise(uint64_t kernel_page, uint64_t kernel_size_pages) {
		//Reserve pages for paging table
		kernel_paging_table = (uint64_t*) PMM::page_number_to_address(PMM::allocate_pages(1, true));
		memset((void*) kernel_paging_table, 0, 4096);

		current_paging_table = kernel_paging_table;
		
		//Identity map first 1MiB (it may be in use by firmware)
		VMM::set_page_translation(0, 0, 256);

		//Identity map kernelspace
		VMM::set_page_translation(kernel_page, kernel_page, kernel_size_pages);

		//Also identity map the framebuffer
		uint64_t framebuffer_page = PMM::address_to_page_number(TextRenderer::graphics_info().address);
		uint64_t framebuffer_page_count = (TextRenderer::graphics_info().buffer_size / 4096) + 1;
		VMM::set_page_translation(framebuffer_page + framebuffer_page_count, framebuffer_page + framebuffer_page_count, PMM::total_pages());
		VMM::set_page_translation(framebuffer_page, framebuffer_page, framebuffer_page_count);
		
		asm("mov %0, %%cr3" : : "r" (current_paging_table)); //Now that we have setup the kernel VAS, we can safely enable paging without messing anything up
	}

	void separate_table_indexes(uint64_t virtual_page, uint16_t *PDP_I, uint16_t *PD_I, uint16_t *PT_I, uint16_t *P_I) {
		*P_I = virtual_page & 0x1ff;
		virtual_page >>= 9;
		*PT_I = virtual_page & 0x1ff;
		virtual_page >>= 9;
		*PD_I = virtual_page & 0x1ff;
		virtual_page >>= 9;
		*PDP_I = virtual_page & 0x1ff;
	}
	
	uint64_t create_table_entry(uint64_t physical_page, uint64_t flags) {
		return (physical_page << 12) | flags;
	}
	
	void set_page_translation(uint64_t physical_page, uint64_t virtual_page, uint64_t page_count) {
		for (uint64_t i = 0; i < page_count; i++)
			recursively_set_table_translation(current_paging_table, physical_page + i, virtual_page + i, 3, TABLE_ENTRY_PRESENT | TABLE_ENTRY_READWRITE);
	}
	
	void recursively_set_table_translation(uint64_t *table, uint64_t physical_page, uint64_t virtual_page, uint8_t table_level, uint64_t flags) {
		uint64_t shift_amount = 9 * table_level;
		uint64_t mask = 0x1FFULL << shift_amount;
		uint64_t table_index = (virtual_page & mask) >> shift_amount;

		//Only the top-level call provides us a normal address, otherwise we have to decode the PTE to find the real address
		if (table_level != 3) {
			table = (uint64_t*) ((((uint64_t) table) >> 12) << 12);
		}

		if (table_level == 0) {
			//This is the bottom of the table hierarchy, we can finally create an entry for the actual page
			table[table_index] = create_table_entry(physical_page, flags);
		} else {
			//Not yet at the bottom of table hierarchy, if no subtable yet allocated, allocate it and create an entry for it in the parent table. Then follow the table down to the bottom
			if (!table[table_index]) {
				uint64_t sub_table = PMM::allocate_pages(1, true);
				memset(PMM::page_number_to_address(sub_table), 0, 4096);
				table[table_index] = create_table_entry(sub_table, flags);
			}

			recursively_set_table_translation((uint64_t*) table[table_index], physical_page, virtual_page, table_level - 1, flags);
		}
	}

}
