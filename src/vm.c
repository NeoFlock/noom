#include "vm.h"
#include "helper.h"
#include "noom.h"
#include <assert.h>
#include <stdio.h>

const noomV_Value noomV_nil = {
    .tag = NOOMV_VNIL,
    .isptr = 0,
    .autoclose = 0,
};

noomV_Value noomV_intVal(noom_int_t i) {
	return (noomV_Value){.tag = NOOMV_VINT, .isptr = 0, .autoclose = 0, .integer = i};
}
noomV_Value noomV_numVal(noom_float_t n) {
	return (noomV_Value){.tag = NOOMV_VNUM, .isptr = 0, .autoclose = 0, .number = n};
}
noomV_Value noomV_boolVal(bool b) {
	return (noomV_Value){.tag = NOOMV_VBOOL, .isptr = 0, .autoclose = 0, .boolean = b};
}

noomV_DisInfo noomV_disInfo[NOOMV_INSTR_NOP2] = {
    [NOOMV_INSTR_PUSHVAL] = {
        .name = "PUSHVAL",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_PUSHCONST] = {
        .name = "PUSHCONST",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_PUSHGLOBAL] = {
        .name = "PUSHGLOBAL",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_PUSHINT] = {
        .name = "PUSHINT",
        .arg = NOOMV_DIS_sD,
    },
    [NOOMV_INSTR_PUSHNIL] = {
        .name = "PUSHNIL",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_PUSHBOOLS] = {
        .name = "PUSHBOOLS",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_PUSHUPVAL] = {
        .name = "PUSHUPVAL",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_PUSHARGS] = {
        .name = "PUSHARGS",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_CREATETABLE] = {
        .name = "CREATETABLE",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_PUSHCLOSURE] = {
        .name = "PUSHCLOSURE",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_CALL] = {
        .name = "CALL",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_GETTABLE] = {
        .name = "GETTABLE",
        .arg = NOOMV_DIS_NONE,
    },
    // pops (table, field, value), and sets `table[field] = value`
    [NOOMV_INSTR_SETTABLE] = {
        .name = "SETTABLE",
        .arg = NOOMV_DIS_NONE,
    },
    [NOOMV_INSTR_SETLIST] = {
        .name = "SETLIST",
        .arg = NOOMV_DIS_uD,
    },
    // pops table, pushes `table[consts[op.uD]]`, an optimization for indexing fields
    [NOOMV_INSTR_GETFIELD] = {
        .name = "GETFIELD",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_SETFIELD] = {
        .name = "SETFIELD",
        .arg = NOOMV_DIS_uD,
    },

    [NOOMV_INSTR_OP] = {
        .name = "OP",
        .arg = NOOMV_DIS_BC,
    },

    [NOOMV_INSTR_SETVAL] = {
        .name = "SETVAL",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_SETUPVAL] = {
        .name = "SETUPVAL",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_SETGLOBAL] = {
        .name = "SETGLOBAL",
        .arg = NOOMV_DIS_uD,
    },

    [NOOMV_INSTR_RET] = {
        .name = "RET",
        .arg = NOOMV_DIS_uD,
    },

    [NOOMV_INSTR_JMP] = {
        .name = "JMP",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_CJMP] = {
        .name = "CJMP",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_CNJMP] = {
        .name = "CNJMP",
        .arg = NOOMV_DIS_uD,
    },

    // For bullshit

    // Take the top value without popping it, push `value[consts[op.uD]]` like GETFIELD would, then swap the top 2 values.
    // This is for a:foo() and such
    [NOOMV_INSTR_GETMETHOD] = {
        .name = "GETMETHOD",
        .arg = NOOMV_DIS_uD,
    },
    // rotate the the top op.a items by op.sD
    [NOOMV_INSTR_ROTATE] = {
        .name = "ROTATE",
        .arg = NOOMV_DIS_sD,
    },
    [NOOMV_INSTR_POP] = {
        .name = "POP",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_INSTR_SETSTACK] = {
        .name = "SETSTACK",
        .arg = NOOMV_DIS_uD,
    },

    [NOOMV_INSTR_CONCAT] = {
        .name = "CONCAT",
        .arg = NOOMV_DIS_uD,
    },

    [NOOMV_INSTR_CLOSE] = {
        .name = "CLOSE",
        .arg = NOOMV_DIS_uD,
    },
};

noomV_Object* noomV_allocObj(noom_LuaVM* vm, noomV_ObjTag tag, noom_uint_t size) {
	noomV_Object* o = noom_alloc(size);
	if (o == 0) return o;

	o->tag = tag;
	o->marked = false;
	o->next = vm->heap;
	o->nextGray = 0;
	vm->heap = o;
	vm->objCount++;
	return o;
}

noomV_String* noomV_allocStr(noom_LuaVM* vm, const char* str, noom_uint_t len) {
	noomV_String* s = (noomV_String*)noomV_allocObj(vm, NOOMV_OSTR, sizeof(noomV_String) + len + 1);
	if (s == 0) return 0;
	noom_memcpy(s->data, str, len);
	s->data[len] = '\0';
	s->len = len;
	s->hash = 0;
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

noomV_Table* noomV_allocTable(noom_LuaVM* vm, noom_uint_t arraylen, noom_uint_t fields) {
	noomV_Table* t = (noomV_Table*)noomV_allocObj(vm, NOOMV_OTABLE, sizeof(noomV_Table));
	if (t == 0) return 0;
	// not computed
	t->len = 0;
	// no metatable
	t->meta = 0;
	// no entries filled
	t->used = 0;
	// bit of space between fields in case of hash collisions to not blast them into O(N) hell
	noom_uint_t cap = fields * 4 + arraylen;
	t->entries = 0;
	t->entrydata = noom_alloc(sizeof(noomV_Value) * 2 * cap);
	// dw about freeing t, its in the VM heap so it'll get GC'd out
	if (t->entrydata == 0) return 0;
	t->entries = cap;
	for (size_t i = 0; i < cap * 2; i++) {
		t->entrydata[i] = noomV_nil;
	}
	return t;
}

noom_uint_t noomV_rawhashValue(noomV_Value v) {
	switch (v.tag) {
		case NOOMV_VNIL:
			return 0;
		case NOOMV_VINT:
			return v.integer;
		case NOOMV_VBOOL:
			return v.boolean ? 1 : 0;
		case NOOMV_VNUM:
			// TODO: see if we should bitcast instead
			return v.number;
		case NOOMV_VCFUNC:
			return (noom_uint_t)v.cfunc;
		case NOOMV_VLUSER:
			return (noom_uint_t)v.lightuserdata;
		case NOOMV_VOBJ: {
			noomV_Object* o = v.obj;
			if (o->tag != NOOMV_OSTR) return (noom_uint_t)o;

			noomV_String* s = (noomV_String*)o;
			if (s->hash == 0) {
				// https://en.wikipedia.org/wiki/Jenkins_hash_function#one_at_a_time
				noom_uint_t hash = 0;
				for (noom_uint_t i = 0; i < s->len; i++) {
					hash += (unsigned char)s->data[i];
					hash += (hash << 10);
					hash ^= (hash >> 6);
				}
				hash += (hash << 3);
				hash ^= (hash >> 11);
				hash += (hash << 15);
				s->hash = hash;
			}
			return s->hash;
		}
	}
}

bool noomV_isNil(noomV_Value key) { return key.tag == NOOMV_VNIL; }

bool noomV_isLegalKey(noomV_Value key) {
	if (key.tag == NOOMV_VNIL) return 0;
	if (key.tag == NOOMV_VNUM) {
		noom_float_t n = key.number;
		// true for all except NaN, which is an illegal key!
		return n == n;
	}
	return 1;
}

bool noomV_rawequalValue(noomV_Value a, noomV_Value b) {
	// special case: integers and numbers
	if (a.tag == NOOMV_VINT && b.tag == NOOMV_VNUM) {
		return (noom_float_t)a.integer == b.number;
	}
	if (a.tag == NOOMV_VNUM && b.tag == NOOMV_VINT) {
		return a.number == (noom_float_t)b.integer;
	}
	if (a.tag != b.tag) return 0;
	switch (a.tag) {
		case NOOMV_VNIL:
			return 1;
		case NOOMV_VINT:
			return a.integer == b.integer;
		case NOOMV_VBOOL:
			return (a.boolean != 0) == (b.boolean != 0);
		case NOOMV_VNUM:
			return a.number == b.number;
		case NOOMV_VCFUNC:
			return a.cfunc == b.cfunc;
		case NOOMV_VLUSER:
			return a.lightuserdata == b.lightuserdata;
		case NOOMV_VOBJ: {
			noomV_Object* aObj = a.obj;
			noomV_Object* bObj = b.obj;
			if (aObj == bObj) return 1;
			if (aObj->tag != bObj->tag) return 0;
			if (aObj->tag != NOOMV_OSTR) return aObj == bObj;

			noomV_String* strA = (noomV_String*)aObj;
			noomV_String* strB = (noomV_String*)bObj;
			if (strA->len != strB->len) return 0;
			if (strA->hash != 0 && strB->hash != 0 && strA->hash != strB->hash) return 0;
			for (noom_uint_t i = 0; i < strA->len; i++) {
				if (strA->data[i] != strB->data[i]) return 0;
			}
			return 1;
		}
	}
}

noomV_Value noomV_rawgetTable(noomV_Table* t, noomV_Value key) {
	if (!noomV_isLegalKey(key)) return noomV_nil;
	noom_uint_t hash = noomV_rawhashValue(key);
	noom_uint_t start = hash % t->entries;
	for (noom_uint_t i = 0; i < t->entries; i++) {
		noom_uint_t idx = (start + i) % t->entries;
		// unallocated entry, bye-bye
		if (t->entrydata[idx].tag == NOOMV_VNIL) break;
		// isptr on a key means its a tombstone
		if (t->entrydata[idx].isptr) continue;
		if (noomV_rawequalValue(t->entrydata[idx], key)) {
			return t->entrydata[idx + t->entries];
		}
	}
	return noomV_nil;
}

noomV_Value noomV_rawgetiTable(noomV_Table* t, noom_int_t idx) {
	return noomV_rawgetTable(t, (noomV_Value){.tag = NOOMV_VINT, .integer = idx});
}

// TODO: implement
// used should be at most ~80% of entrydata because
// that's standard practice and we're not computer scientists
noom_Exit noomV_rawsetTable(noom_LuaVM* vm, noomV_Table* t, noomV_Value key, noomV_Value val) {
	if (!noomV_isLegalKey(key)) {
		noomV_setErrorStr(vm, vm->currentThread, "illegal key");
		return NOOM_ERUNTIME;
	}
	noom_uint_t target = t->entries * 80 / 100;
	if (t->used > target) {
		// TODO: resize hashtable
	}
	// prevent awkward bugs
	key.isptr = 0;
	key.autoclose = 0;
	val.isptr = 0;
	val.autoclose = 0;
	noom_uint_t hash = noomV_rawhashValue(key);
	noom_uint_t idx = hash % t->entries;
	noom_uint_t count = 0;
	noomV_Value *freshTomb = 0;
	while (count < t->entries) {
		noomV_Value key2 = t->entrydata[idx];
		// tombstone, for creation!
		if(key2.isptr) {
			if(freshTomb == 0) freshTomb = t->entrydata + idx;
			idx++;
			idx %= t->entries;
			count++;
			continue;
		}
		// unallocated!
		if (key2.tag == NOOMV_VNIL) {
			t->entrydata[idx] = key;
			t->entrydata[idx + t->entries] = val;
			return NOOM_OK;
		}
		if (noomV_rawequalValue(key, key2)) {
			// HOLY SHIET!!!!!
			t->entrydata[idx + t->entries] = val;
			return NOOM_OK;
		}
		idx++;
		idx %= t->entries;
		count++;
	}
	if(freshTomb == 0) return NOOM_ENOMEM;
	freshTomb[0] = key;
	freshTomb[t->entries] = val;
	return NOOM_OK;
}

noom_uint_t noomV_rawlenTable(noomV_Table* t) {
	noom_uint_t l = 0, h = t->len;
    
	while (l < h) {
		const noom_uint_t mid = l + (h - l) / 2 + 1; 
		if (noomV_isNil(noomV_rawgetiTable(t, (noom_int_t)mid))) h = mid - 1;
		else l = mid;
	}
    
	return t->len = l;
}

noomV_Thread* noomV_allocCoroutine(noom_LuaVM* vm) {
	noomV_Thread* thrd = (noomV_Thread*)noomV_allocObj(vm, NOOMV_OTHREAD, sizeof(noomV_Thread));
	if (thrd == 0) return 0;
	thrd->resumedBy = 0;
	thrd->resuming = 0;
	thrd->calldepth = 0;
	thrd->stacklen = 0;
	thrd->stackcap = 32;
	thrd->callcap = 4;
	thrd->errObj = noomV_nil;
	thrd->yielded = -1;

	thrd->calls = noom_alloc(sizeof(noomV_CallFrame) * thrd->callcap);
	if (thrd->calls == 0) return 0;
	thrd->stack = noom_alloc(sizeof(noomV_Value) * thrd->stackcap);
	if (thrd->stack == 0) return 0;

	return thrd;
}

void noomV_setErrorStr(noom_LuaVM* vm, noomV_Thread* coro, const char* str) {
	noomV_String* s = noomV_allocStr(vm, str, noom_strlen(str));
	if (s == 0) {
		coro->errObj = vm->oomVal;
		return;
	}
	coro->errObj.tag = NOOMV_VOBJ;
	coro->errObj.obj = &s->obj;
}

noom_Exit noomV_setErrorFromExit(noom_LuaVM* vm, noomV_Thread* coro, noom_Exit exit) {
	switch (exit) {
		case NOOM_OK:
			coro->errObj = noomV_nil;
			break;
		case NOOM_EINTERNAL:
			noomV_setErrorStr(vm, coro, "unrecoverable internal error");
			break;
		case NOOM_ENOMEM:
			coro->errObj = vm->oomVal;
			break;
		case NOOM_ERUNTIME:
			break;
		case NOOM_ENOSTACK:
			noomV_setErrorStr(vm, coro, "stack overflow");
			break;
		case NOOM_EIO:
			noomV_setErrorStr(vm, coro, "I/O error");
			break;
		case NOOM_ELIMIT:
			noomV_setErrorStr(vm, coro, "buffer overflow");
			break;
		case NOOM_EGOTONOLABEL:
			noomV_setErrorStr(vm, coro, "goto without a label");
			break;
		case NOOM_EERROR:
			break;
	}
	return exit;
}

void noomV_freeObj(noom_LuaVM* vm, noomV_Object* obj) {
	if (obj->tag == NOOMV_OFUNC) {
		noomV_Function* f = (noomV_Function*)obj;
		noom_free(f->consts);
		noom_free(f->upvals);
		noom_free(f->locals);
		noom_free(f->code);
	}
	if (obj->tag == NOOMV_OTABLE) {
		noomV_Table* t = (noomV_Table*)obj;
		noom_free(t->entrydata);
	}
	if (obj->tag == NOOMV_OTHREAD) {
		noomV_Thread* thrd = (noomV_Thread*)obj;
		noom_free(thrd->stack);
		noom_free(thrd->calls);
	}
	noom_free(obj);
	vm->objCount--;
}

noom_Exit noomV_pushCallFrame(noom_LuaVM* vm, noomV_Thread* coro, noomV_CallFrame cf) {
	if (coro->calldepth >= NOOM_MAXCALL) {
		noomV_setErrorStr(vm, coro, "stack overflow");
		return NOOM_ENOSTACK;
	}
	if (coro->calldepth >= coro->callcap) {
		noom_uint_t newCap = coro->callcap * 2;
		noomV_CallFrame* newCalls = noom_realloc(coro->calls, sizeof(noomV_CallFrame) * newCap);
		if (newCalls == 0) return NOOM_ENOMEM;
		coro->calls = newCalls;
		coro->callcap = newCap;
	}
	coro->calls[coro->calldepth++] = cf;
	return NOOM_OK;
}

noomV_CallFrame* noomV_topCallFrame(noomV_Thread* coro) {
	if (coro->calldepth == 0) return NULL;
	return &coro->calls[coro->calldepth - 1];
}

noom_Exit noomV_setThreadStackSize(noom_LuaVM* vm, noomV_Thread* coro, noom_int_t stack) {
	if (stack < 0) {
		noomV_setErrorStr(vm, coro, "stack underflow");
		return NOOM_ERUNTIME;
	}
	if (stack > NOOM_MAXSTACK) return NOOM_ELIMIT;
	if (stack > coro->stackcap) {
		noom_uint_t newCap = coro->stackcap;
		while (newCap < stack) newCap *= 2;

		noomV_Value* newStack = noom_realloc(coro->stack, sizeof(noomV_Value) * coro->stackcap);
		if (newStack == 0) return NOOM_ENOMEM;
		coro->stack = newStack;
		coro->stackcap = newCap;
	}

	// NIL-ify
	if (coro->stacklen < stack) {
		for (int i = coro->stacklen; i < stack; i++) {
			coro->stack[i] = noomV_nil;
		}
		coro->stacklen = stack;
		return NOOM_OK;
	}
	// close. TODO: actually loop to run __close!
	// Preferably figure out some way to do a linked list of TBC values
	coro->stacklen = stack;
	return NOOM_OK;
}

noom_LuaVM* noom_createVM(noom_LuaVersion version) {
	noom_LuaVM* vm = noom_alloc(sizeof(*vm));
	if (vm == 0) return 0;
	// initialize the universe to NULL, handles partial OOMs nicely
	vm->heap = 0;
	vm->graySet = 0;
	vm->oomVal.tag = NOOMV_VOBJ;
	vm->oomVal.obj = 0;
	vm->globals = 0;
	vm->registry = 0;
	vm->mainThread = 0;
	vm->currentThread = 0;
	vm->version = version;
	vm->objCount = 0;
	vm->gcTarget = 100;
	vm->gcRatio = 2;

	const char* oomStr = "out of memory";
	vm->oomVal.obj = (noomV_Object*)noomV_allocStr(vm, oomStr, noom_strlen(oomStr));
	if (vm->oomVal.obj == 0) goto rip;
	vm->registry = noomV_allocTable(vm, 0, 1);
	if (vm->registry == 0) goto rip;
	vm->globals = noomV_allocTable(vm, 0, 32);
	if (vm->globals == 0) goto rip;

	vm->mainThread = noomV_allocCoroutine(vm);
	if (vm->mainThread == 0) goto rip;

	vm->currentThread = vm->mainThread;

	// goal: put _G in the registry as the _G key
	noomV_String* _G = noomV_allocStr(vm, "_G", 2);
	if (_G == 0) goto rip;

	noomV_Value _GKey = {.tag = NOOMV_VOBJ, .obj = &_G->obj, .isptr = 0, .autoclose = 0};
	noomV_Value _GVal = {.tag = NOOMV_VOBJ, .obj = &vm->globals->obj, .isptr = 0, .autoclose = 0};

	noom_Exit err = noomV_rawsetTable(vm, vm->registry, _GKey, _GVal);
	if (err) goto rip;

	assert(noomV_rawequalValue(noomV_rawgetTable(vm->registry, _GKey), _GVal));

	noom_gc(vm);

	return vm;
rip:
	noom_destroyVM(vm);
	return 0;
}

void noom_destroyVM(noom_LuaVM* vm) {
	noomV_Object* iter = vm->heap;
	while (iter) {
		noomV_Object* cur = iter;
		iter = iter->next;
		noomV_freeObj(vm, cur);
	}
	noom_free(vm);
}

noom_Exit noomV_pushRawValue(noom_LuaVM *vm, noomV_Value val) {
	noomV_Thread *coro = vm->currentThread;
	if (coro->stacklen >= NOOM_MAXSTACK) {
		noomV_setErrorStr(vm, coro, "stack overflow");
		return NOOM_ENOSTACK;
	}
	noom_Exit e = noom_pushnil(vm);
	if(e) return e;
	val.isptr = 0;
	coro->stack[coro->stacklen-1] = val;
	return NOOM_OK;
}

noom_Exit noomV_pushRawObject(noom_LuaVM *vm, noomV_Object *obj) {
	noomV_Value v = noomV_nil;
	v.tag = NOOMV_VOBJ;
	v.obj = obj;
	return noomV_pushRawValue(vm, v);
}

noom_Exit noomV_getStackValue(noom_LuaVM *vm, noom_slot_t slot, noomV_Value *outVal) {
	noomV_Thread *curThread = vm->currentThread;
	size_t stacksize = noom_getstacksize(vm);
	if(slot < 0) slot += stacksize;
	if(slot < 0) {
		noomV_setErrorStr(vm, curThread, "stack underflow");
		return NOOM_ERUNTIME;
	}
	if(slot >= stacksize) {
		noomV_setErrorStr(vm, curThread, "stack overflow");
		return NOOM_ENOSTACK;
	}
	if(curThread->calldepth == 0) {
		*outVal = curThread->stack[slot];
	} else {
		noomV_CallFrame *cf = noomV_topCallFrame(curThread);
		*outVal = curThread->stack[cf->funcIdx+1+slot];
	}
	if(outVal->isptr) {
		*outVal = outVal->ptr->value;
	}
	return NOOM_OK;
}

noom_Exit noomV_setStackValue(noom_LuaVM *vm, noom_slot_t slot, noomV_Value val) {
	noomV_Thread *curThread = vm->currentThread;
	size_t stacksize = noom_getstacksize(vm);
	if(slot < 0) slot += stacksize;
	if(slot < 0) {
		noomV_setErrorStr(vm, curThread, "stack underflow");
		return NOOM_ERUNTIME;
	}
	if(slot >= stacksize) {
		noomV_setErrorStr(vm, curThread, "stack overflow");
		return NOOM_ENOSTACK;
	}

	noom_uint_t idx;
	if(curThread->calldepth == 0) {
		idx = slot;
	} else {
		noomV_CallFrame *cf = noomV_topCallFrame(curThread);
		idx = cf->funcIdx + 1 + slot;
	}

	noomV_Value *target = &curThread->stack[idx];
	if (target->isptr) {
		target->ptr->value = val;
		return NOOM_OK;
	}

	val.isptr = 0;
	*target = val;
	return NOOM_OK;
}

noom_int_t noom_getstacksize(noom_LuaVM* vm) {
	noomV_Thread *cur = vm->currentThread;
	if(cur->calldepth == 0) return cur->stacklen;
	noomV_CallFrame *cf = noomV_topCallFrame(cur);
	return cur->stacklen - cf->funcIdx - 1;
}

noom_Exit noom_setstacksize(noom_LuaVM* vm, noom_int_t amount) {
	noomV_Thread *cur = vm->currentThread;
	if(amount < 0) {
		noomV_setErrorStr(vm, cur, "stack underflow");
		return NOOM_ERUNTIME;
	}
	if(cur->calldepth == 0) return noomV_setThreadStackSize(vm, cur, amount);
	noomV_CallFrame *cf = noomV_topCallFrame(cur);
	return noomV_setThreadStackSize(vm, cur, cf->funcIdx + amount + 1);
}

// runs Lua code. May call C code.
noom_Exit noomV_runCodeResumed(noom_LuaVM *vm, noom_Exit status, void *_ctx) {
	// error while resumed
	if(status) return status;
	noomV_Thread *thrd = vm->currentThread;
	size_t pointOfGivingUp = (size_t)_ctx;

	// The interpreter is really just a more complex C function

	while(1) {
		// we yielded
		if(thrd->yielded >= 0) return NOOM_OK;
		
		noomV_CallFrame *cf = noomV_topCallFrame(thrd);
		// some mf ran this on C code, idiot!
		if(cf->isC) return NOOM_EINTERNAL;

		// TODO: use jmp table for performance!!!
		unsigned int varargc = cf->varargc;
		noomV_Function *proto = cf->proto;
		if(cf->pc == 0) {
			// rotate to position all varargs at the start
		}
		noomV_Inst inst = proto->code[cf->pc++];
		noom_Exit e = NOOM_OK;

		// NOTE: if we want fully resumable Lua code, we gotta figure out how to make error handlers and hooks resumable in this system!
		// This will probably required more info in callframes and perhaps a more complicated C API.

		switch(inst.op) {
		case NOOMV_INSTR_NOP:
		case NOOMV_INSTR_NOP2:
			break;
		case NOOMV_INSTR_PUSHVAL:
			{
				noomV_Value v;
				e = noomV_getStackValue(vm, inst.us + varargc, &v);
				if(e) return e;
				e = noomV_pushRawValue(vm, v);
				if(e) return e;
			}
			break;
		case NOOMV_INSTR_PUSHCONST:
			{
				e = noomV_pushRawValue(vm, proto->consts[inst.us]);
				if(e) return e;
			}
			break;
		case NOOMV_INSTR_PUSHGLOBAL:
			{
				if(vm->version != NOOM_VERSION_51) return NOOM_EINTERNAL;
				noomV_Value g = noomV_rawgetTable(proto->env, proto->consts[inst.us]);
				thrd->stacklen--;
				e = noomV_pushRawValue(vm, g);
				if(e) return e;
			}
			break;
		case NOOMV_INSTR_PUSHINT:
			{
				e = noom_pushint(vm, inst.ss);
				if(e) return e;
			}
			break;
		case NOOMV_INSTR_PUSHNIL:
			{
				e = noom_pushnils(vm, inst.us+1);
				if(e) return e;
			}
			break;
		case NOOMV_INSTR_PUSHBOOLS:
			{
				return NOOM_EINTERNAL;
			}
			break;
		case NOOMV_INSTR_PUSHUPVAL:
			{
				e = noomV_pushRawValue(vm, cf->upvals[inst.us]->value);
				if(e) return e;
			}
			break;
		case NOOMV_INSTR_PUSHARGS:
			{
				return NOOM_EINTERNAL;
			}
			break;
		case NOOMV_INSTR_CREATETABLE:
			{
				return NOOM_EINTERNAL;
			}
			break;
		case NOOMV_INSTR_PUSHCLOSURE:
			{
				return NOOM_EINTERNAL;
			}
			break;
		case NOOMV_INSTR_CALL:
			{
				// TODO: call whenever noom_callk() is fully implemented
				return NOOM_EINTERNAL;
			}
			break;
		case NOOMV_INSTR_RET:
			{
				// handled by surrounding noom_callk()
				return noom_return(vm, noom_getstacksize(vm) - inst.us);
			}
			break;
		default:
			{
				return NOOM_EINTERNAL;
			}
			break;
		}
	}
}

noom_Exit noom_pushint(noom_LuaVM* vm, noom_int_t integer) {
	noomV_Value v = noomV_nil;
	v.tag = NOOMV_VINT;
	v.integer = integer;
	return noomV_pushRawValue(vm, v);
}

noom_Exit noom_callk(noom_LuaVM* vm, noom_uint_t argc, noom_int_t retc, noom_KFunction* f, void* ctx) {
	noomV_Thread *thrd = vm->currentThread;
	if(noom_getstacksize(vm) < argc+1) {
		noomV_setErrorStr(vm, thrd, "stack underflow");
		return NOOM_ERUNTIME;
	}
	// TODO: check for __call
	if(noom_typeof(vm, -argc - 1) != NOOM_LUA_TFUNCTION) {
		printf("check\n");
		noomV_setErrorStr(vm, thrd, "attempt to call non-function value");
		return NOOM_ERUNTIME;
	}
	if(thrd == vm->mainThread && f != NULL) {
		noomV_setErrorStr(vm, thrd, "main thread cannot be resumed");
		return NOOM_ERUNTIME;
	}
	noomV_CallFrame *topCall = noomV_topCallFrame(thrd);
	noomV_Value v;
	noom_Exit e = noomV_getStackValue(vm, -argc - 1, &v);
	// TODO: C functions and closures
	noomV_CallFrame cf = {
		.funcIdx = thrd->stacklen - argc - 1,
		.returnCount = 0,
		.isC = false,
		.errhandler = noomV_nil,
		.upvals = NULL,
		.resumeFunc = NULL,
		.resumeCtx = NULL,
		.proto = NULL,
		.pc = 0,
		.varargc = 0,
	};
	// just assume function, burn with fire later
	noomV_Function *func = (noomV_Function *)v.obj;
	cf.proto = func;
	cf.resumeCtx = (void *)(size_t)thrd->calldepth;
	cf.resumeFunc = noomV_runCodeResumed;
	e = noomV_pushCallFrame(vm, thrd, cf);
	if(e) return e;
	// TODO: fix argument count.
	if(topCall != NULL) {
		topCall->resumeFunc = f;
		topCall->resumeCtx = ctx;
	}
	e = cf.resumeFunc(vm, e, cf.resumeCtx);
	if(topCall != NULL) {
		topCall->resumeFunc = NULL;
		topCall->resumeCtx = NULL;
	}
	// TODO: handle returns.
	return e;
}

noom_Exit noom_return(noom_LuaVM* vm, noom_uint_t retc) {
	noomV_Thread *thrd = vm->currentThread;
	if(thrd->calldepth == 0) return NOOM_OK;
	noomV_CallFrame *cf = noomV_topCallFrame(thrd);
	cf->returnCount = retc;
	return NOOM_OK;
}

noom_LuaType noom_typeof(noom_LuaVM* vm, noom_slot_t x) {
	int curstack = noom_getstacksize(vm);
	if(x >= curstack || x < -curstack) {
		printf("typeof: %d, %d\n", (int)x, curstack);
		return NOOM_LUA_TNIL;
	}
	noomV_Value v;
	noomV_getStackValue(vm, x, &v);
	switch(v.tag) {
	case NOOMV_VNIL:
		return NOOM_LUA_TNIL;
	case NOOMV_VINT:
		return NOOM_LUA_TNUM;
	case NOOMV_VNUM:
		return NOOM_LUA_TNUM;
	case NOOMV_VBOOL:
		return NOOM_LUA_TBOOL;
	case NOOMV_VCFUNC:
		return NOOM_LUA_TFUNCTION;
	case NOOMV_VLUSER:
		return NOOM_LUA_TUSERDATA;
	case NOOMV_VOBJ:
		{
			noomV_Object *obj = v.obj;
			switch(obj->tag) {
			case NOOMV_OSTR:
				return NOOM_LUA_TSTR;
			case NOOMV_OFUNC:
				return NOOM_LUA_TFUNCTION;
			case NOOMV_OTABLE:
				return NOOM_LUA_TTABLE;
			case NOOMV_OPOINTER:
				// ???????
				return NOOM_LUA_TUSERDATA;
			case NOOMV_OTHREAD:
				return NOOM_LUA_TTHREAD;
			case NOOMV_OUSERDATA:
				return NOOM_LUA_TUSERDATA;
			case NOOMV_OCLOSURE:
				return NOOM_LUA_TFUNCTION;
			}
		}
	}
}
