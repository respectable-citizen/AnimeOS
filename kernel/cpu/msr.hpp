#pragma once

#include <stdint.h>

namespace CPU {
	namespace MSR {
		uint64_t read(uint32_t);
		void write(uint32_t, uint64_t);
	}
}
