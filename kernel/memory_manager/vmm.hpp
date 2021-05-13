#pragma once

#include <stdint.h>

#define KERNEL_MEMORY_PAGES 524288 //The first KERNEL_MEMORY_PAGES of memory are identity mapped and used solely for the kernel

namespace VMM {
	namespace PageFlag {
		enum {
			Present = 1 << 0,
			ReadWrite = 1 << 1,
			WT = 1 << 3,
			PAT = 1 << 7,
			Default = Present | ReadWrite
		};
	}

	enum class CachingMode {
		WriteBack,
		WriteCombine
	};

	void initialise(void*, void*);

	void map(void*, void*, uint64_t, CachingMode);
	void map_space(void*, void*, void*, uint64_t, CachingMode);
	//void* allocate_pages(uint64_t);
};
