#include "isr.hpp"

#include "text_renderer/text_renderer.hpp"

#include "utils.hpp"

__attribute((interrupt)) void isr8(void *p) {
	TextRenderer::kernel_panic((char*) "Double fault occurred.");
	UNUSED(p);
}

__attribute((interrupt)) void isr14(void *p) {
	TextRenderer::kernel_panic((char*) "Page fault occurred.");
	UNUSED(p);
}

__attribute((interrupt)) void isr128(void *p) {
	TextRenderer::kernel_panic((char*) "SYSCALL interrupt.");
	UNUSED(p);
}
