#include "noom.h"
#include "vm.h"

static void noomG_grayObj(noom_LuaVM* vm, noomV_Object* obj);

static void noomG_grayValue(noom_LuaVM* vm, noomV_Value value) {
	if (value.tag != NOOMV_VOBJ) return;
	noomG_grayObj(vm, value.obj);
}

static void noomG_grayObj(noom_LuaVM* vm, noomV_Object* obj) {
	if (obj == 0) return;
	if (obj->marked) return;
	obj->marked = 1;
	obj->nextGray = vm->graySet;
	vm->graySet = obj;
}

void noomG_markObj(noom_LuaVM* vm, noomV_Object* obj) {
	if (obj->tag == NOOMV_OPOINTER) {
		noomV_Pointer* p = (noomV_Pointer*)obj;
		noomG_grayValue(vm, p->value);
		return;
	}
	if (obj->tag == NOOMV_OFUNC) {
		noomV_Function* f = (noomV_Function*)obj;
		noomG_grayObj(vm, &f->chunkname->obj);
		noomG_grayObj(vm, &f->env->obj);
		for (noom_uint_t i = 0; i < f->constsize; i++) {
			noomG_grayValue(vm, f->consts[i]);
		}
		for (noom_uint_t i = 0; i < f->protosize; i++) {
			noomG_grayObj(vm, (noomV_Object*)f->protos[i]);
		}
		return;
	}
	if (obj->tag == NOOMV_OTABLE) {
		noomV_Table* t = (noomV_Table*)obj;
		noomG_grayObj(vm, (noomV_Object*)t->meta);
		for (noom_uint_t i = 0; i < t->entries * 2; i++) {
			noomG_grayValue(vm, t->entrydata[i]);
		}
		return;
	}
	if (obj->tag == NOOMV_OTHREAD) {
		noomV_Thread* thrd = (noomV_Thread*)obj;
		for (noom_uint_t i = 0; i < thrd->stacklen; i++) {
			noomG_grayValue(vm, thrd->stack[i]);
		}
		for (noom_uint_t i = 0; i < thrd->calldepth; i++) {
			noomV_CallFrame cf = thrd->calls[i];
			noomG_grayValue(vm, cf.errhandler);
		}
		noomG_grayValue(vm, thrd->errObj);
		noomG_grayObj(vm, (noomV_Object*)thrd->resuming);
		noomG_grayObj(vm, (noomV_Object*)thrd->resumedBy);
		return;
	}
}

void noom_gc(noom_LuaVM* vm) {
	vm->graySet = 0;

	noomG_grayObj(vm, (noomV_Object*)vm->mainThread);
	noomG_grayObj(vm, (noomV_Object*)vm->globals);
	noomG_grayObj(vm, (noomV_Object*)vm->registry);

	while (vm->graySet) {
		noomG_markObj(vm, vm->graySet);
		vm->graySet = vm->graySet->nextGray;
	}

	noomV_Object** sweep = &vm->heap;
	while (*sweep) {
		noomV_Object* current = *sweep;
		if (current->marked) {
			sweep = &current->next;
		} else {
			*sweep = current->next;
			noomV_freeObj(vm, current);
		}
	}

	vm->gcTarget = vm->objCount * vm->gcRatio;
}

void noom_trygc(noom_LuaVM* vm) {
	if (vm->objCount >= vm->gcTarget) noom_gc(vm);
}
