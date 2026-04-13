#include "types.h"

int noom_startswith(const char* str, char* compare);
int noom_streql(const char* stra, noom_uint_t lena, const char* strb, noom_uint_t lenb); // rename to something better?

void* noom_alloc(noom_uint_t size);
void noom_free(void* ptr);
void* noom_realloc(void* ptr, noom_uint_t size);
