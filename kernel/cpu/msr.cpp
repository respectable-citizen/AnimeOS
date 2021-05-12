#include "msr.hpp"

#include "../text_renderer/text_renderer.hpp"

namespace CPU {
	namespace MSR {

		uint64_t read(uint32_t msr) {
			uint32_t low, high;
			asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr) : "memory");
			return ((uint64_t)high << 32) | (uint64_t)low;
		}

		void write(uint32_t msr, uint64_t value) {
			uint32_t low = value;
			uint32_t high = value >> 32;
			//TextRenderer::draw_number((high << 32) | low);
			//TextRenderer::draw_string((char*) "\r\n");
			//for (;;);
			asm volatile("wrmsr" : : "a"((uint32_t) low), "d"((uint32_t) high), "c"(msr) : "memory");
		}

	}
}
