#include "noom.h"
#include "vm.h"

#define NOOM_USHORT_MAX 0xffff

noomV_Value noom_tonumber_except_different_name_so_public_fucking_api_works(const char* s, const char** endptr, noom_LuaVersion luauauauaua);
double noom_pow(double x, int y);

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
void *noom_realloc_free(void* ptr, noom_uint_t size);
