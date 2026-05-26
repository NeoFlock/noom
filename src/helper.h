#include "types.h"

int noom_startswith(const char* str, const char* compare);
int noom_streql(const char* stra, noom_uint_t lena, const char* strb, noom_uint_t lenb); // rename to something better?
noom_uint_t noom_strlen(const char *s);
int noom_strcmp(const char *a, const char* b);
void noom_safe_strcpy(char* buffer, noom_uint_t* pos, noom_uint_t buffer_size, const char* src);

void* noom_alloc(noom_uint_t size);
void noom_free(void* ptr);
void* noom_realloc(void* ptr, noom_uint_t size);
