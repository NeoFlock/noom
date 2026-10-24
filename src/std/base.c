#include "noom.h"
#include <stdio.h>

noom_Exit noom_base_print(noom_LuaVM* vm) {
	noom_Exit result;
	
	noom_uint_t stack_size = noom_getstacksize(vm);

	const char tab = '\t';
	const char nl = '\n';	

	for (noom_uint_t i = 0; i < stack_size; i++) {
		// if ((result = noom_cast2str(vm, i))) return result;
		const char* s;
		noom_uint_t len;


		if ((result = noom_tolstring(vm, i, &s, &len))) return result;

		fwrite(s, sizeof(char), len, stdout);
		fwrite(&tab, sizeof(char), 1, stdout);
	}

	fwrite(&nl, sizeof(char), 1, stdout);

	return NOOM_OK;
}

noom_Exit noom_openbase(noom_LuaVM* vm) {
	noom_Exit result;
	
	if ((result = noom_pushglobals(vm))) return result;
	if ((result = noom_pushstring(vm, "print"))) return result;
	if ((result = noom_pushcfunction(vm, noom_base_print))) return result;
	if ((result = noom_settable(vm))) return result;

	return NOOM_OK;
}
