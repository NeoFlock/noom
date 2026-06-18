#include "helper.h"

#include "noom.h"

int noom_startswith(const char* str, const char* compare) {
#ifdef __has_builtin
#if __has_builtin(__builtin_strncmp)
	return __builtin_strncmp(compare, str, noom_strlen(compare)) == 0;
#endif
#endif
	noom_uint_t i = 0;
	while (1) {
		if (compare[i] == '\0') return 1; // we did it
		if (str[i] != compare[i]) return 0;
		i++;
	}
	return 0; // unreachable but whatevs
}

int noom_memeq(const char* stra, noom_uint_t lena, const char* strb, noom_uint_t lenb) {
	if (lena != lenb) return 0;

	for (noom_uint_t i = 0; i < lena; i++) {
		if (stra[i] != strb[i]) return 0;
	}

	return 1;
}

noom_uint_t noom_strlen(const char* s) {
#ifdef __has_builtin
#if __has_builtin(__builtin_strlen)
	return __builtin_strlen(s);
#endif
#endif
	const char* a = s;
	while (*s) s++;
	return s - a;
}

int noom_strcmp(const char* a, const char* b) {
#ifdef __has_builtin
#if __has_builtin(__builtin_strcmp)
	return __builtin_strcmp(a, b);
#endif
#endif
	for (; *a && *a == *b; a++, b++);
	return *(const unsigned char*)a - *(const unsigned char*)b;
}

void noom_safe_strcpy(char* buffer, noom_uint_t* pos, noom_uint_t buffer_size, const char* src) {
	while (*src && *pos < buffer_size - 1) {
		buffer[(*pos)++] = *src++;
	}
}

char* noom_strndup(const char* s, const noom_uint_t len) {
	char* whar = noom_alloc(len + 1);
	if (whar == 0) return 0;
	noom_memcpy(whar, s, len);
	whar[len] = '\0';
	return whar;
}

void noom_memcpy(void* dst, const void* src, noom_uint_t n) {
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy)
	__builtin_memcpy(dst, src, n);
	return;
#endif
#endif
	unsigned char* d = dst;
	const unsigned char* s = src;

	while (n--) {
		*d++ = *s++;
	}
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
