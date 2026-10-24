#include "helper.h"
#include "lexer.h"
#include "noom.h"

static bool noom_parse_exponent(const char** sp, int* out) {
	const char* s = *sp;
	const bool neg = *s == '-';
	if (*s == '-' || *s == '+') s++;
	if (!noomL_isnumber(*s)) return false;

	int e = 0;
	while (noomL_isnumber(*s)) {
		e = e * 10 + (*s - '0');
		s++;
	}
	*out = neg ? -e : e;
	*sp = s;
	return true;
}

static noom_float_t noom_pow(double x, int y) {
	if (y == 0)
		return 1;

	const noom_float_t temp = noom_pow(x, y / 2);

	if (y % 2 == 0) {
		return temp * temp;
	}

	if (y > 0)
		return x * temp * temp;

	return temp * temp / x;
}

static noom_float_t noom_scale(noom_float_t v, noom_float_t base, int e, int step) {
	if (v == 0.0 || e == 0) return v;
	if (e > 0) {
		if (e > step) {
			v *= noom_pow(base, step);
			e -= step;
		}
		return v * noom_pow(base, e);
	} else {
		e = -e;
		if (e > step) {
			v /= noom_pow(base, step);
			e -= step;
		}
		return v / noom_pow(base, e);
	}
}

noomV_Value noom_tonumber_except_different_name_so_public_fucking_api_works(const char* s, const char** endptr, noom_LuaVersion luauauauaua) {
	// Num???? Is that a fucking noom reference???????
	const char* const start = s;
	#define NOOM_FAIL() do { if (endptr) *endptr = start; return (noomV_Value){.tag = NOOMV_VNIL}; } while (0)

	const bool negative = *s == '-';
	if (*s == '-' || *s == '+') s++;

	if (!noomL_isnumber(s[*s == '.'])) NOOM_FAIL();

	const bool i_can_have_an_int = luauauauaua >= NOOM_VERSION_53;

	if (*s == '0' && noomL_lower(s[1]) == 'x') {
		s += 2;

		uint64_t ival = 0;
		noom_float_t num = 0.0;
		int exp2 = 0;
		int sig = 0;
		bool any = false, is_float = false;

		while (noomL_ishex(*s)) {
			const int d = noomL_isnumber(*s) ? *s - '0' : noomL_lower(*s) - 'a' + 10;
			ival = ival * 16u + (uint64_t)d;
			if (sig < 30) {
				num = num * 16.0 + d;
				if (num != 0.0) sig++;
			} else exp2 += 4;
			any = true;
			s++;
		}
		if (*s == '.') {
			is_float = true;
			s++;
			while (noomL_ishex(*s)) {
				const int d = noomL_isnumber(*s) ? *s - '0' : noomL_lower(*s) - 'a' + 10;
				if (sig < 30) {
					num = num * 16.0 + d;
					if (num != 0.0) sig++;
					exp2 -= 4;
				}
				any = true;
				s++;
			}
		}
		if (!any) NOOM_FAIL(); 

		if (noomL_lower(*s) == 'p') {
			s++;
			int e;
			if (!noom_parse_exponent(&s, &e)) NOOM_FAIL();
			exp2 += e;
			is_float = true;
		}

		if (endptr) *endptr = s;

		if (i_can_have_an_int && !is_float) {
			if (negative) ival = 0u - ival;
			return (noomV_Value){ .tag = NOOMV_VINT, .integer = (noom_int_t)ival };
		}
		num = noom_scale(num, 2.0, exp2, 1000);
		return (noomV_Value){.tag = NOOMV_VNUM, .number = negative ? -num : num};
	}

	uint64_t mant = 0;
	int ndig = 0;
	int exp10 = 0;
	bool any = false, is_float = false;

	uint64_t ival = 0;
	const uint64_t maxby10 = (uint64_t)INT64_MAX / 10;
	const int maxlastd = (uint64_t)INT64_MAX % 10;

	while (noomL_isnumber(*s)) {
		const int d = *s - '0';
		if (ndig < 19) {
			mant = mant * 10u + (uint64_t)d;
			if (mant) ndig++;
		} else exp10++;

		if (!is_float) {
			if (ival > maxby10 || (ival == maxby10 && d > maxlastd + (negative ? 1 : 0)))
				is_float = true;
			else
				ival = ival * 10u + (uint64_t)d;
		}
		any = true;
		s++;
	}
	if (*s == '.') {
		is_float = true;
		s++;
		while (noomL_isnumber(*s)) {
			const int d = *s - '0';
			if (ndig < 19) {
				mant = mant * 10u + (uint64_t)d;
				if (mant) ndig++;
				exp10--;
			}
			any = true;
			s++;
		}
	}
	if (!any) NOOM_FAIL();

	if (noomL_lower(*s) == 'e') {
		s++;
		int e;
		if (!noom_parse_exponent(&s, &e)) NOOM_FAIL();
		exp10 += e;
		is_float = true;
	}

	if (endptr) *endptr = s;

	if (i_can_have_an_int && !is_float) {
		const uint64_t r = negative ? (0u - ival) : ival;
		return (noomV_Value){ .tag = NOOMV_VINT, .integer = (noom_int_t)r };
	}

	noom_float_t num = noom_scale((noom_float_t)mant, 10.0, exp10, 300);
	return (noomV_Value){ .tag = NOOMV_VNUM, .number = negative ? -num : num };
#undef NOOM_FAIL
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
