#include "noom.h"

#define NOOM_USHORT_MAX 0xffff

int noom_startswith(const char* str, const char* compare);
int noom_memeq(const char* stra, noom_uint_t lena, const char* strb, noom_uint_t lenb);
noom_uint_t noom_strlen(const char* s);
int noom_strcmp(const char* a, const char* b);
void noom_safe_strcpy(char* buffer, noom_uint_t* pos, noom_uint_t buffer_size, const char* src);
char* noom_strndup(const char* s, noom_uint_t len);

void noom_memcpy(void* dst, const void* src, noom_uint_t n);

void* noom_alloc(noom_uint_t size);
void noom_free(void* ptr);
void* noom_realloc(void* ptr, noom_uint_t size);
