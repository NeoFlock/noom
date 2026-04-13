#include "helper.h"
#include "types.h"

int noom_startswith(const char* str, char* compare) {
	noom_uint_t i = 0;
	while (1) {
		if (compare[i] == '\0') return 1; // we did it
		if (str[i] != compare[i]) return 0;
		i++;
	}
	return 0; // unreachable but whatevs
}

int noom_streql(const char* stra, noom_uint_t lena, const char* strb, noom_uint_t lenb) {
	if (lena != lenb) return 0;

	for (noom_uint_t i = 0; i < lena; i++) {
		if (stra[i] != strb[i]) return 0;
	}

	return 1;
}

#include <stdlib.h> // TODO: remove

void* noom_alloc(noom_uint_t size) {
	return malloc(size);
}

void noom_free(void* ptr) {
	free(ptr);
}

void* noom_realloc(void* ptr, noom_uint_t size) {
	return realloc(ptr, size);
}
