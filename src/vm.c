#include "vm.h"
#include "helper.h"

noomV_Object *noomV_allocObj(noom_LuaVM *vm, noomV_ObjTag tag, noom_uint_t size) {
	noomV_Object *o = noom_alloc(size);
	if(o == 0) return o;

	o->tag = tag;
	o->marked = 0;
	o->next = vm->heap;
	o->nextGray = 0;
	vm->heap = o;
	return o;
}

void noomV_freeObj(noomV_Object *obj) {
	noom_free(obj);
}

noom_LuaVM *noom_createVM(noom_LuaVersion version) {
	noom_LuaVM *vm = noom_alloc(sizeof(*vm));
	if(vm == 0) return 0;
	// initialize the universe to NULL, handles partial OOMs nicely
	vm->heap = 0;
	vm->graySet = 0;
	vm->globals = 0;
	vm->registry = 0;
	vm->mainThread = 0;
	vm->currentThread = 0;
	vm->version = version;
	return vm;
}

void noom_destroyVM(noom_LuaVM *vm) {
	noomV_Object *iter = vm->heap;
	while(iter) {
		noomV_Object *cur = iter;
		iter = iter->next;
		noomV_freeObj(cur);
	}
	noom_free(vm);
}
