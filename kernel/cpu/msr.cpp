#include "msr.hpp"

#include "../text_renderer/text_renderer.hpp"

namespace CPU {
	namespace MSR {
		uint64_t read(uint32_t msr) {
			uint32_t low;
			uint32_t high;
			asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
			return ((uint64_t) high << 32) | low;
		}

		void write(uint32_t msr, uint64_t value) {
			uint64_t high = value >> 32;
			uint64_t low = value - (high << 32);
			TextRenderer::draw_number((high << 32) | low);
			TextRenderer::draw_string((char*) "\r\n");
			//for (;;);
			asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
		}
	}
}
