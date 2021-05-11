#include "cpuid.hpp"

#include "../text_renderer/text_renderer.hpp"

namespace CPU {
	namespace CPUID {
		void check_cpu_features() {
			if (check_cpuid_supported()) {
				//Get supported features
				uint32_t edx_features;
				request(1, nullptr, nullptr, nullptr, &edx_features);
				
				if (((edx_features >> 5) & 1) == 0) {
					TextRenderer::kernel_panic((char*) "CPU does not support MSRs");
				}

				if (((edx_features >> 16) & 1) == 0) {
					TextRenderer::kernel_panic((char*) "CPU does not support PAT");
				}
			} else {
				TextRenderer::kernel_panic((char*) "CPU does not support CPUID instruction");
			}
		}
	}
}
