#include "utils.hpp"

void hang() {
	asm("cli");
	for (;;) asm("hlt");
}
