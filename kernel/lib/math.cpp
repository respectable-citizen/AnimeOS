#include "math.hpp"

uint64_t log(uint64_t n) {
	return (n > 1) ? 1 + log(n / 2) : 0; 
} 

uint64_t log_with_base(uint64_t n, uint64_t base) {
	return log(n) / log(base);
} 
