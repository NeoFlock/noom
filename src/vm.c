#include "vm.h"
#include "helper.h"

noomV_Object* noomV_allocObj(noom_LuaVM* vm, noomV_ObjTag tag, noom_uint_t size) {
	noomV_Object* o = noom_alloc(size);
	if (o == 0) return o;

	o->tag = tag;
	o->marked = 0;
	o->next = vm->heap;
	o->nextGray = 0;
	vm->heap = o;
	return o;
}

noomV_String* noomV_allocStr(noom_LuaVM* vm, const char* str, noom_uint_t len) {
	noomV_String* s = (noomV_String*)noomV_allocObj(vm, NOOMV_OSTR, sizeof(noomV_String) + len + 1);
	if (s == 0) return 0;
	noom_memcpy(s->data, str, len);
	s->data[len] = '\0';
	return s;
}

noomV_Function* noomV_allocFunc(noom_LuaVM* vm, noomV_String* chunkname) {
	noomV_Function* f = (noomV_Function*)noomV_allocObj(vm, NOOMV_OFUNC, sizeof(noomV_Function));
	if (f == 0) return 0;
	f->chunkname = chunkname;
	f->env = 0;
	f->code = 0;
	f->consts = 0;
	f->protos = 0;
	f->upvals = 0;
	f->locals = 0;
	f->codesize = 0;
	f->linedefined = 0;
	f->lastlinedefined = 0;
	f->argc = 0;
	f->flags = 0;
	f->constsize = 0;
	f->protosize = 0;
	f->upvalsize = 0;
	f->localsize = 0;
	return f;
}

noomV_Table* noomV_allocTable(noom_LuaVM* vm) {
	noomV_Table* t = (noomV_Table*)noomV_allocObj(vm, NOOMV_OTABLE, sizeof(noomV_Table));
	if (t == 0) return 0;
	t->meta = 0;
	t->entries = 0;
	t->used = 0;
	t->len = 0;
	t->entrydata = 0;
	return t;
}

void noomV_freeObj(noomV_Object* obj) {
	noom_free(obj);
}

noom_LuaVM* noom_createVM(noom_LuaVersion version) {
	noom_LuaVM* vm = noom_alloc(sizeof(*vm));
	if (vm == 0) return 0;
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

void noom_destroyVM(noom_LuaVM* vm) {
	noomV_Object* iter = vm->heap;
	while (iter) {
		noomV_Object* cur = iter;
		iter = iter->next;
		noomV_freeObj(cur);
	}
	noom_free(vm);
}
