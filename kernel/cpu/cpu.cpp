#include "cpu.hpp"
#include "cpuid.hpp"
#include "msr.hpp"

#include "../text_renderer/text_renderer.hpp"

namespace CPU {
	void initialise() {
		CPUID::check_cpu_features();
		uint64_t value = MSR::read(0x277);
		TextRenderer::draw_number(value);
		TextRenderer::draw_string((char*) "\r\n");
		uint8_t *values = (uint8_t*) &value;
		values[4] = 1;
		values[5] = 1;
		values[6] = 1;
		values[7] = 1;
		TextRenderer::draw_number(value);
		TextRenderer::draw_string((char*) "\r\n");
		MSR::write(0x277, value);

		value = MSR::read(0x277);
		TextRenderer::draw_number(value);
		TextRenderer::draw_string((char*) "\r\n");
	}
}
