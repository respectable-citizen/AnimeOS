#include "memory_manager.hpp"

#include "text_renderer/text_renderer.hpp"

namespace MemoryManager {
	namespace {
		/*
		Bits 0-5 are not in use
		Bit 6 is free flag
		Bit 7 is reserved flag
		*/

		uint8_t *m_page_map;

		//These fields are purely informational, they are kept up to date by the corresponding functions
		uint64_t m_total_pages;
		uint64_t m_free_pages;
		uint64_t m_reserved_pages;
	}

	void initialise(MemoryMap memory_map, uint64_t kernel_start, uint64_t kernel_end) {
		m_total_pages = 0;
		m_free_pages = 0;
		m_reserved_pages = 0;

		//Count how many pages there are, and find largest memory segment. Then determine if the largest memory segment is big enough to store our paging information
		uint32_t descriptor_count = memory_map.memory_map_size / memory_map.descriptor_size;

		uint64_t largest_segment_size = 0;
		uint8_t *largest_segment;
		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR*) (((uint8_t*) memory_map.memory_descriptors) + (memory_map.descriptor_size * i));
			m_total_pages += descriptor->NumberOfPages;

			if (descriptor->Type == EfiConventionalMemory && (descriptor->NumberOfPages > largest_segment_size)) {
				largest_segment_size = descriptor->NumberOfPages;
				largest_segment = (uint8_t*) descriptor->PhysicalStart;
			}
		}
	
		if (m_total_pages > (largest_segment_size * 4096)) {
			//There isn't enough space in the largest segment to store the page information
			//I don't think this would ever happen, but, just in case it does, error
			TextRenderer::kernel_panic((char*) "Not enough space to store page map.");
		}
	
		m_page_map = largest_segment;

		uint64_t current_page = 0;
		for (uint32_t i = 0; i < descriptor_count; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR*) (((uint8_t*) memory_map.memory_descriptors) + (memory_map.descriptor_size * i));
			uint8_t page_type = PAGE_FREE;
			
			if (descriptor->Type == EfiConventionalMemory) {
				m_free_pages += descriptor->NumberOfPages;
			} else {
				m_reserved_pages += descriptor->NumberOfPages;
				page_type = PAGE_RESERVED;
			}
			
			for (uint64_t j = 0; j < descriptor->NumberOfPages; j++) {
				m_page_map[current_page + j] = page_type;
			}

			current_page += descriptor->NumberOfPages;
		}

		//Mark page map as reserved memory
		uint64_t pages_required_for_page_map = (m_total_pages / 4096) + 1;
		reserve_pages(address_to_page_number(m_page_map), pages_required_for_page_map);

		//Mark kernelspace as reserved memory
		uint64_t kernel_size_pages = address_to_page_number((void*) kernel_end) - address_to_page_number((void*) kernel_start) + 1;
		reserve_pages(address_to_page_number((void*) kernel_start), kernel_size_pages);
	}

	uint64_t free_pages() {
		return m_free_pages;		
	}

	uint64_t reserved_pages() {
		return m_reserved_pages;		
	}

	uint64_t used_pages() {
		return total_pages() - free_pages();
	}

	uint64_t total_pages() {
		return m_total_pages;
	}

	uint8_t get_page(uint64_t page) {
		return m_page_map[page];
	}

	uint64_t find_contiguous_pages(uint64_t page_count) {
		uint64_t contiguous_pages = 0;
		uint64_t page_start;
		for (uint64_t i = 0; i < m_total_pages; i++) {
			if ((m_page_map[i] & PAGE_FREE) && !(m_page_map[i] & PAGE_RESERVED)) {
				contiguous_pages++;
				if (contiguous_pages == 1) page_start = i;
				if (contiguous_pages == page_count) break;
			} else {
				contiguous_pages = 0;

			}
		}

		return page_start;
	}

	uint64_t modify_pages(uint64_t start_page, uint64_t page_count, uint8_t attributes) {
		uint64_t total_modified_pages = 0;
		for (uint64_t i = 0; i < page_count; i++) {
			if (get_page(start_page + i) != attributes) total_modified_pages++;
			
			m_page_map[start_page + i] = attributes;
		}
		return total_modified_pages;
	}

	void lock_pages(uint64_t start_page, uint64_t page_count) {
		m_free_pages -= modify_pages(start_page, page_count, PAGE_LOCKED);
	}
	
	void free_pages(uint64_t start_page, uint64_t page_count) {
		m_free_pages += modify_pages(start_page, page_count, PAGE_FREE);
	}
	
	void reserve_pages(uint64_t start_page, uint64_t page_count) {
		uint64_t modified_pages = modify_pages(start_page, page_count, PAGE_RESERVED);
		m_reserved_pages += modified_pages;
		m_free_pages -= modified_pages;
	}
	
	void unreserve_pages(uint64_t start_page, uint64_t page_count) {
		uint64_t modified_pages = modify_pages(start_page, page_count, PAGE_FREE);
		m_reserved_pages -= modified_pages;
		m_free_pages += modified_pages;
	}

	uint64_t address_to_page_number(void *address) {
		return ((uint64_t) address) / 4096;
	}

	void* page_number_to_address(uint64_t page_number) {
		return (void*) (page_number * 4096);
	}
}
