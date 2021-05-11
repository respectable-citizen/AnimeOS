#pragma once

#include <stdint.h>

namespace CPU {
	namespace CPUID {
		static inline void request(uint32_t code, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
			//This is a pretty convoluted way of ignoring parameterse if they are a nullptr, is there a better way to do this?
			uint32_t t_eax;
			uint32_t t_ebx;
			uint32_t t_ecx;
			uint32_t t_edx;
			asm("cpuid":"=a"(t_eax),"=b"(t_ebx),"=c"(t_ecx),"=d"(t_edx):"a"(code));
			if (eax) *eax = t_eax;
			if (ebx) *ebx = t_ebx;
			if (ecx) *ecx = t_ecx;
			if (edx) *edx = t_edx;
		}

		static inline void request_string(uint32_t code, char *str) {
			request(code, (uint32_t*) str, (uint32_t*) (str + 4), (uint32_t*) (str + 8), (uint32_t*) (str + 12));
		}
		
		extern "C" bool check_cpuid_supported();
		void check_cpu_features();
	}
}
