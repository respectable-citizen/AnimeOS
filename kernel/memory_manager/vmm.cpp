#include "vmm.hpp"

#include "pmm.hpp"

#include "text_renderer/text_renderer.hpp"
#include "lib/memory.hpp"

#include "utils.hpp"

#define REMOVE_FLAGS 0xFFFFF000

namespace VMM {
	namespace {
		uint64_t *m_PML4;		
	}

	void initialise(void *kernel_start, void *kernel_end) {
		//Allocate page memory for PML4
		m_PML4 = (uint64_t*) PMM::alloc_page();
		memset((void*) m_PML4, 0, 4096);
		
		//Identity map required amount of pages for kernel
		map_space(kernel_start, kernel_end, kernel_start, PageFlag::Default, CachingMode::WriteBack);
		
		//Identity map the framebuffer
		void *framebuffer_start = TextRenderer::graphics_info().address;
		void *framebuffer_end = (void*) ((uint64_t) framebuffer_start + (uint64_t) TextRenderer::graphics_info().buffer_size);
		map_space(framebuffer_start, framebuffer_end, framebuffer_start, PageFlag::Default, CachingMode::WriteBack);
		
		//Identity map the PMM page map
		void *page_map_start = PMM::page_map();
		void *page_map_end = (void*) ((uint64_t) page_map_start + (PMM::pages_for_page_map() * 4096));
		map_space(page_map_start, page_map_end, page_map_start, PageFlag::Default, CachingMode::WriteCombine);

		asm("mov %0, %%cr3" : : "r" (m_PML4)); //Now that we have setup all our required pages, we can safely enable paging without messing anything up
	}

	void map(void *physical_address, void *virtual_address, uint64_t flags, CachingMode caching_mode) {
		uint64_t vaddr = (uint64_t) virtual_address;
		vaddr >>= 12;
		uint16_t PML1_index = vaddr & 0x1ff;
		vaddr >>= 9;
		uint16_t PML2_index = vaddr & 0x1ff;
		vaddr >>= 9;
		uint16_t PML3_index = vaddr & 0x1ff;
		vaddr >>= 9;
		uint16_t PML4_index = vaddr & 0x1ff;

		uint64_t *PML3 = (uint64_t*) (m_PML4[PML4_index] & REMOVE_FLAGS);
		if (!PML3) {
			PML3 = (uint64_t*) PMM::alloc_page();
			memset((void*) PML3, 0, 4096);
			m_PML4[PML4_index] = ((uint64_t) PML3) | flags;
		}

		uint64_t *PML2 = (uint64_t*) (PML3[PML3_index] & REMOVE_FLAGS);
		if (!PML2) {
			PML2 = (uint64_t*) PMM::alloc_page();
			memset((void*) PML2, 0, 4096);
			PML3[PML3_index] = ((uint64_t) PML2) | flags;
		}

		uint64_t *PML1 = (uint64_t*) (PML2[PML2_index] & REMOVE_FLAGS);
		if (!PML1) {
			PML1 = (uint64_t*) PMM::alloc_page();
			memset((void*) PML1, 0, 4096);
			PML2[PML2_index] = ((uint64_t) PML1) | flags;
		}

		if (caching_mode == CachingMode::WriteCombine) {
			flags |= PageFlag::PAT; //Write combining is the 5th item in the PAT
		}

		PML1[PML1_index] = ((uint64_t) physical_address) | flags;
	}
	
	void map_space(void *physical_start, void *physical_end, void *virtual_address, uint64_t flags, CachingMode caching_mode) {
		while (physical_start < physical_end) {
			map(physical_start, virtual_address, flags, caching_mode);
			
			physical_start = (void*) ((uint64_t) physical_start + 4096);
			virtual_address = (void*) ((uint64_t) virtual_address + 4096);
		}
	}
/*
	void* allocate_pages(uint64_t page_count) {
		for (uint64_t i = 0; i < page_count; i++) {
			void *physical_page_address = PMM::alloc_page();

		}
	}*/
}
