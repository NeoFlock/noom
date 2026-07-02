#include "helper.h"
#include "lexer.h"
#include "noom.h"

// TODO make it actually output integers if the lua version allows that uhhhhh
noomV_Value noom_tonumber_except_different_name_so_public_fucking_api_works(const char* s, const char** endptr, noom_LuaVersion luauauauaua) {
	// Num???? Is that a fucking noom reference???????
	noom_float_t num = 0.0;

	const noom_bool_t negative = *s == '-';
	if (*s == '-' || *s == '+')
		s++;

	if (!noomL_isnumber(s[*s == '.'])) return (noomV_Value){ .tag = NOOMV_VNIL };

	if (*s == '0' && noomL_lower(s[1]) == 'x') {
		s += 2;
		// if the string starts with "0x" but does not contain digits it's invalid
		noom_bool_t error = 1;
		while (noomL_ishex(*s)) {
			int digit;
			if (noomL_isnumber(*s)) digit = *s - '0';
			else digit = noomL_lower(*s) - 'a' + 10;

			num = num * 16.0 + digit;
			error = 0;
			s++;
		}

		if (error) return (noomV_Value){ .tag = NOOMV_VNIL };

		if (noomL_lower(*s) == 'p') {
			// damn then
			s++;
			int exponent = 0;
			const noom_bool_t exponent_negative = *s == '-';
			if (*s == '-' || *s == '+')
				s++;

			if (!noomL_isnumber(*s)) return (noomV_Value){ .tag = NOOMV_VNIL };
			
			while (noomL_isnumber(*s)) {
				exponent = exponent * 10 + (*s - '0');
				s++;
			}

			num *= noom_pow(2.0, exponent_negative ? -exponent : exponent);
		}
	} else {
		noom_bool_t error = 1;
		while (noomL_isnumber(*s)) {
			num = num * 10.0 + (*s - '0');
			error = 0;
			s++;
		}
		if (*s == '.') {
			s++;

			double fraction_divisor = 10.0;
			while (noomL_isnumber(*s)) {
				error = 0;
				num += (double)(*s - '0') / fraction_divisor;
				fraction_divisor *= 10.0;
				s++;
			}
		}
		if (error) return (noomV_Value){ .tag = NOOMV_VNIL };

		if (noomL_lower(*s) == 'e') {
			s++;
			int exponent = 0;
			const noom_bool_t exponent_negative = *s == '-';
			if (*s == '-' || *s == '+')
				s++;

			if (!noomL_isnumber(*s)) return (noomV_Value){ .tag = NOOMV_VNIL };

			while (noomL_isnumber(*s)) {
				exponent = exponent * 10 + (*s - '0');
				s++;
			}

			num *= noom_pow(10.0, exponent_negative ? -exponent : exponent);
		}
	}

	if (endptr) *endptr = s;
	return (noomV_Value){ .tag = NOOMV_VNUM, .number = negative ? -num : num };
}

// made by some stranger on stack overflow
double noom_pow(double x, int y) {
	if (y == 0)
		return 1;

	const double temp = noom_pow(x, y / 2);

	if (y % 2 == 0) {
		return temp * temp;
	}

	if (y > 0)
		return x * temp * temp;

	return temp * temp / x;
}

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

void* noom_realloc_free(void* ptr, noom_uint_t size) {
	void* newptr = noom_realloc(ptr, size);
	if (newptr == 0) {
		noom_free(ptr);
		return 0;
	}
	return newptr;
}
