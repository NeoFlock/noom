#include "vm.h"
#include "helper.h"
#include <assert.h>
#include <stdio.h>

const noomV_Value noomV_nil = {
    .tag = NOOMV_VNIL,
    .isptr = 0,
    .autoclose = 0,
};

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
    [NOOMV_INSTR_GETUPFIELD] = {
        .name = "GETUPFIELD",
        .arg = NOOMV_DIS_uD,
    },
    [NOOMV_ISNTR_SETUPFIELD] = {
        .name = "SETUPFIELD",
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
	o->marked = 0;
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

noom_bool_t noomV_isNil(noomV_Value key) { return key.tag == NOOMV_VNIL; }

noom_bool_t noomV_isLegalKey(noomV_Value key) {
	if (key.tag == NOOMV_VNIL) return 0;
	if (key.tag == NOOMV_VNUM) {
		noom_float_t n = key.number;
		// true for all except NaN, which is an illegal key!
		return n == n;
	}
	return 1;
}

noom_bool_t noomV_rawequalValue(noomV_Value a, noomV_Value b) {
	// special case: integers and numbers
	if (a.tag == NOOMV_VINT && b.tag == NOOMV_VNUM) {
		return a.integer == b.number;
	}
	if (a.tag == NOOMV_VNUM && b.tag == NOOMV_VINT) {
		return a.number == b.integer;
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
			freshTomb = t->entrydata + idx;
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
	noom_uint_t guess = t->len;
	// TODO: optimize with a bsearch
	while (1) {
		if (!noomV_isNil(noomV_rawgetiTable(t, guess + 1))) guess++;
		if (guess == 0) break;
		if (noomV_isNil(noomV_rawgetiTable(t, guess))) guess--;
	}
	return t->len = guess;
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
		case NOOM_PLEASEHELPMEIAMSCARED:
			noomV_setErrorStr(vm, coro, "temporary error");
			break;
		case NOOM_IAMNOTSCAREDJUSTLAZYTHISTIME:
			noomV_setErrorStr(vm, coro, "programmer laziness error");
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

noom_Exit noomV_pushCallFrame(noom_LuaVM* vm, noomV_Thread* coro, noomV_CallFrame cf);

noomV_CallFrame* noomV_topCallFrame(noomV_Thread* coro) {
	if (coro->calldepth == 0) return 0;
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
