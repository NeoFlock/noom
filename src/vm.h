#ifndef NOOM_VM
#define NOOM_VM

#include "noom.h"

#ifndef NOOM_MAXSTACK
#define NOOM_MAXSTACK 4096
#endif

#ifndef NOOM_MAXCALL
#define NOOM_MAXCALL (NOOM_MAXSTACK / 32)
#endif

// Defines values, threads, bullshit, idc

typedef enum noomV_ObjTag {
	NOOMV_OSTR,
	NOOMV_OFUNC,
	NOOMV_OCLOSURE,
	NOOMV_OTABLE,
	NOOMV_OUSERDATA,
	NOOMV_OPOINTER,
} noomV_ObjTag;

typedef struct noomV_Object {
	noomV_ObjTag tag;
	noom_bool_t marked;
	struct noomV_Object* next;
	struct noomV_Object* nextGray;
} noomV_Object;

typedef enum noomV_ValueTag : unsigned char {
	NOOMV_VNIL,
	NOOMV_VBOOL,
	NOOMV_VINT,
	NOOMV_VNUM,
	NOOMV_VLUSER,
	NOOMV_VCFUNC,
	NOOMV_VOBJ,
} noomV_ValueTag;

typedef struct noomV_Value {
	noomV_ValueTag tag;
	// for stack slots
	noom_bool_t autoclose;
	// pointer to value
	noom_bool_t isptr;
	union {
		noom_bool_t boolean;
		noom_int_t integer;
		noom_float_t number;
		void* lightuserdata;
		noomV_Object* obj;
		struct noomV_Pointer* ptr;
	};
} noomV_Value;

typedef struct noomV_String {
	noomV_Object obj;
	noom_uint_t len;
	// Includes NUL-terminator!
	char data[];
} noomV_String;

typedef struct noomV_Table {
	noomV_Object obj;
	struct noomV_Table* meta;
	// amount of entries allocated
	noom_uint_t entries;
	// how many entries are defined.
	// Note, this includes keys with null values, which are still scanned.
	noom_uint_t used;
	// cache of #t, for faster subsequent fetches
	noom_uint_t len;
	// actual values. Given index i, V[i] is the key and V[i+entries] is the value.
	// This is because we mostly read the key, so we should put more keys in cache.
	noomV_Value* entrydata;
} noomV_Table;

typedef struct noomV_Pointer {
	noomV_Object obj;
	noomV_Value value;
} noomV_Pointer;

typedef enum noomV_Opcode : unsigned char {
	NOOMV_INSTR_NOP = 0,

	// Push data on the stack

	// Pushes the value at `stack[op.uD]`
	NOOMV_INSTR_PUSHVAL,
	// Pushes `consts[op.uD]`
	NOOMV_INSTR_PUSHCONST,
	// Pushes `_G[consts[op.uD]]`, Lua 5.1 only.
	NOOMV_INSTR_PUSHGLOBAL,
	// Pushes op.sD itself as an integer
	NOOMV_INSTR_PUSHINT,
	// Pushes [op.uD+1] nils
	NOOMV_INSTR_PUSHNIL,
	// Pushes [op.a+1] booleans, encoded in [op.uD], where the ith (0-indexed) boolean is encoded in `uD & (1 << i)`
	NOOMV_INSTR_PUSHBOOLS,
	// Pushes *upvals[op.uD]
	NOOMV_INSTR_PUSHUPVAL,
	// Pushes op.a varargs. If op.a is 0, it will push all of them, regardless of how many
	NOOMV_INSTR_PUSHARGS,
	// Pushes a new table. op.uD is the initial capacity
	NOOMV_INSTR_CREATETABLE,
	// Pushes a closure, using the prototype in protos[op.uD]
	NOOMV_INSTR_PUSHCLOSURE,

	// High-level ops

	// Call the function stored at stack[op.a]. All values after that are treated at arguments. op.uD is the expected return count plus one, and if op.uD is 0,
	// all returns are pushed.
	NOOMV_INSTR_CALL,

	// pops (table, field), and pushes table[field]
	NOOMV_INSTR_GETTABLE,
	// pops (table, field, value), and sets table[field] = value
	NOOMV_INSTR_SETTABLE,
	// pops op.uD values, then sets it as an array starting at op.a+1 in the table at the new top;
	// if that is confused, it basically does this:
	// int table = stacksize - op.uD - 1;
	// for(int i = 0; i < op.uD; i++) stack[table][op.a + 1 + i] = stack[table + i + 1];
	NOOMV_INSTR_SETLIST,
	// pops table, pushes table[consts[op.uD]], an optimization for indexing fields
	NOOMV_INSTR_GETFIELD,
	// pops (table, value), sets table[consts[op.uD]] = value, an optimization for changing fields
	NOOMV_INSTR_SETFIELD,

	// does a unary or binary operation.
	// if op.a == 1, its a unary operation.
	// if op.a == 2, its a binary operation.
	// op.b stores the operation type.
	NOOMV_INSTR_OP,

	// pops value, sets stack[op.uD] = value
	NOOMV_INSTR_SETVAL,
	// pops value, sets *upvals[op.uD] = value
	NOOMV_INSTR_SETUPVAL,
	// pops value, sets _G[consts[op.uD]] = value
	NOOMV_INSTR_SETGLOBAL,

	// Control flow

	// returns from a function. The amount returned is from op.uD
	NOOMV_INSTR_RET,

	// computes T = op.a << 16 + op.uD, jumps to T.
	NOOMV_INSTR_JMP,
	// pops value, if value is truthy, does the same as JMP.
	NOOMV_INSTR_CJMP,
	// pops value, if value is falsy, does the same as JMP.
	NOOMV_INSTR_CNJMP,

	// For bullshit

	// Take the top value without popping it, push value[consts[op.uD]] like GETFIELD would, then swap the top 2 values.
	// This is for a:foo() and such
	NOOMV_INSTR_GETMETHOD,
	// rotate the the top op.a items by op.sD
	NOOMV_INSTR_ROTATE,
	// pop op.uD values
	NOOMV_INSTR_POP,
	// set the stack size to op.uD, inserting nils if need be, good for labels
	NOOMV_INSTR_SETSTACK,

	// pop and concatenate the top [op.uD+1] values to a string and push it
	NOOMV_INSTR_CONCAT,

	// mark stack slot op.uD as to-be-closed
	NOOMV_INSTR_CLOSE,

	NOOMV_INSTR_NOP2 = 0xff,
} noomV_Opcode;

typedef struct noomV_Inst {
	noomV_Opcode op;
	unsigned char a;
	union {
		struct {
			unsigned char b;
			unsigned char c;
		};
		short ss;
		unsigned short us;
	};
} noomV_Inst;

typedef struct noomV_UpvalDesc {
	char* name;
	unsigned char idx;
	// whether the index is a stack index
	noom_bool_t isStack;
} noomV_UpvalDesc;

typedef struct noomV_LocalDesc {
	char* name;
	unsigned char stackIdx;
	// to forbid changing it with debug.setlocal
	noom_bool_t isConst;
	// offset of first instruction where local exists
	unsigned int pcStart;
	// offset of first instruction where local is dropped
	unsigned int pcEnd;
} noomV_LocalDesc;

typedef struct noomV_Function {
	noomV_Object obj;
	// source location string
	noomV_String* chunkname;
	// globals. Lua 5.1 only, use the _ENV upvalue for Lua 5.2+
	noomV_Table* env;
	noomV_Inst* code;
	noomV_Value* consts;
	struct noomV_Function** protos;
	noomV_UpvalDesc* upvals;
	noomV_LocalDesc* locals;
	unsigned int codesize;
	// line of function
	unsigned int linedefined;
	// line of end
	unsigned int lastlinedefined;
	// very size limited
	//       ░░░░░░░░░                                                         ░ ▒░░▒    ▒░   ░░  ░
	//       ░░░░░░░░░                                                         █░█░▓▓   ░█▒████▒░ █░░████▓
	//       ░░░░░░░░░                                                        ▓████████░█░▒████▓░████  ░█░
	//       ░░░░░░░░░           ░░░                                         ░█▓░█▒█░█░▒▒█▓████▒ ░█▒█ ░█░░
	//       ░░░░░░░░░         ░░░░░                                         ░█▓▓███▒█  ▓▓▒▒▒▓▓▒ ▒█▒█▓██▓▓
	//       ░░░░░░░░░           ░░                                          ░▓▓███▓█▒░░█▒▓█▓██▓ ▒▓█▓▓▓█▒▒
	//       ░░░░░░░░░░          ░░                                           ▒▓█░█▒█░ ░▓▓██████░░██░░▒█
	//       ░░░░░░░░░░    ░░░░  ░░                                           ▓▓█▒█▓█▒░ ▒▒▒█▒░█░  ███░░█
	//       ░░░░░░░░░░          ░░                                           ▓▒▓███░█▒ ▒░░░▓▓█░░██░▓▒██
	//       ░░░░░░░░░░          ░░        ░                                  ░░  ░░ ░░ ░░   ▒░ ░░░  ░░
	//       ░░░░░░░░░░  ░       ░░░       ░                                    ░ ░                  ░
	//       ░░░░░░░░░░    ░░░░  ░░        ░
	// ░     ░░░░░░░░░     ░░░░                      ░         ░  ░         ░░     ░░░   ░░░ ░    ░░
	//       ░░░░░░░░░        ░             █████  ▒████  ░▓███▓░ ░████░ ░████▓  ░████░ ░▓███▓  ░████░
	// ░░     ░░░░░░░░░                      █▓▒██░░██▓██▒░░██▒██░░██▒▓██░▒██▒██  ██▓▓██ ░██▒██░ ██▒▓██
	// ░░    ░░░░░░░░░░                         ██░▒█▓ ░██░██▒ ▒█▒░██ ░██ ██░ ▓█▒░██ ░██░██░ ▓█▒░██  ██░
	//     ░░░░░░░░░░░                      ░▓███░▒█▓  ██░██░ ▒██▒██ ░██░██  ▒█▒░██ ░██░██░ ▒█▓▒█▓ ░██░
	//      ░░░░░░░░░░        ░     ░▒███░  ▓████░▓█▒ ░██░██░ ▒██▒█▓ ░██░██ ░▒█▒░██ ░██░██░ ░██▒█▓ ░██░░
	//       ░░░░░░░░░     ░░ ░              ░░██▓▓█▓░░██ ██░ ▒█▓▒█▓  ██░██░ ▒█▒░██ ░██░██░ ░██▒█▓ ░██░░
	//      ░░░░░░░░░░   ░░░░               ░ ░███▒██ ░██ ██▒ ▓█▒▒██ ░██░██░ ██░░██░░██ ██▒ ▓█░░██ ░██
	//       ░░░░░░░░░   ░░░               ░█████▒░█████▒▒▓█████ ░█████▒ ▒█████▓██████▓ ▒██▓██░ █████▒
	//       ░░░░░░░░░   ░░                ░████▒ ░░███▒░█▒▒███░  ░███▓   ▓███░▓█░███▓░  ▒███░  ░███▒
	//       ░░░░░░░░░░                                 ░█░    ░   ░           ▒▒          ░
	//       ░░░░░░░░░                                   ░                                           ░
	//       ░░░░░░░░░   ░░░░
	//       ░░░░░░░░░   ░░░░░                     ░░                 ░          ░                 ░    ░
	//       ░░░░░░░░░   ░░   ░░▒░░               ░▓▒       ░█▓       ░                            ░    ░
	//       ░░░░░░░░░   ░░  ▒█████░              ░██       ░██     ░                               ░
	//      ░░░░░░░░░░░░░    ██▒▒███   ░░   ░ ░ ░░ ░░       ░██
	//   ░  ░░░░░░░░░░░░░░░  ██░ ░▓▓ ░████  ░████░░█▓ ▒███▓░░██
	//  ░░░░░░░░░░░░░░░░░░░░░▓███░░ ░██████░██▓██▓▒██░██▓██▒▒██░
	// ░░░░░░░░░░░░░░░░░░░░   ░████░░█▓  ██░██  ▓▓░██ ░░▒██▓▒██░     ░
	// ░░░░░░░░░░░░░░░░░░░   ░░░░███▒█▓  ██▒██   ░░██░█████▓▒██░    ░░         ░
	//  ░░░░░░░░░░░░░░░░░░  ░██▒ ░██▒██░░██░██ ░█▓░██░██░▓█▓░██░    ░          ██▓████████▓▒██▒
	//   ░░░░░░░░░░░░░░░░░  ░██████▓ █████▓ █████▓▒██░█████▓░██░    ░          ░░░███████████▓█▓
	//    ░░░░░░░░░░░░░░░░░░ ░▓███▒  ░███▒  ░▓██▓ ░█▓ ▒██▓█▓░██░                 ░████████████▓█░
	//    ░░░░░░░░░░░░░░░░░  ░                             ░                        ░█████████▓██
	//    ░░░░░░░░░░░░░░ ░░                                                           ▒███████▓█▓
	//   ░ ░░░░░░░░░░░░░░   ░░░░                                                      ██▓██▓▓▓███   ░
	//   ░░░░░░░░░░░░░░░     ░░           ░                                          ░███████████
	//     ░░░░░░░░░░░░    ░░                        ░                               ████████████    ▒
	//     ░░░░░░░░░░░░       ░░▒▒░░         ░      ░██░▓█░                    ░   ░░████ ▒██████    █
	//      ░░░░░░░░░░░       ▓█████░               ░██ ██░░█▓░                     ▓████  ▓█████   ░█  ▒▒
	//      ░░░░░░░░░░░      ▒██░░██▒░░░░░ ░░░░░ ░░░░██  ░░░██░                     █████  ▒█████   ▒█ ▒█░
	//    ░░░░░░░░░░░░░      ██▒  ░█▓░████░████░ ░█████ ██▒████                     █████  ▓████▒   ████░
	//    ░░░░░░░░░░░░       ██░     ▒███▓██▓▓██ ██▒▒██ ██░▒██░                     ████▒  █████ ▒████████
	//      ░░░░░░░░░░       ██░  ░  ▒██░ ██▒▒██░██ ░██ ██ ░██░                    ░████  ░█████   ▓██▓▒░░
	//      ░░░░░░░░░░       ██▒  ░▒░▒██ ░██████▒██ ░██░██░░██                     ▒███▒  █████▒
	//   ░   ░░░░░░░░░       ▓█▓░ ▓█▓▒██ ░██░ ░░░██ ░██ ██░░██░                    ░███░░░█████
	//       ░░░░░░░░░        ██████ ▒█▓  ▓█████ ██████ ██░░███░                   ░██▒  █████▒
	//       ░░░░░░░░░        ░▓███░░░█▓ ░░▒███░░░██▓█▓ ▓█░ ▒█▓░                    ▓▓  ░█████
	//       ░░░░░░░░░         ░░░░          ░░         ░░ ░ ░                      ▓▒  ▓████░
	//       ░░░░░░░░            ░░          ░░      ░                              ▒▓  ████▒
	//       ░ ░░░░░ ░                 ░                                             ▒░▓███▒
	//         ░░░░             ░                                                    ░▓▓██
	//          ░░░
	//          ░░░
	//          ░░
	unsigned char argc;
	unsigned char flags;
	unsigned short constsize;
	unsigned char protosize;
	unsigned char upvalsize;
	unsigned char localsize;
} noomV_Function;

typedef struct noomV_CallFrame {
	// stack index of function
	unsigned int funcIdx;
	noom_bool_t isC;
	union {
		struct {
			//
			unsigned int pc;
			// amount of varargs
			unsigned int varargc;
		} lua;
		struct {
			noom_KFunction* resumeFunc;
			void* resumeCtx;
		} c;
	};
} noomV_CallFrame;

typedef struct noomV_Thread {
	noomV_Object obj;
	unsigned int stacklen, calldepth;
	unsigned int stackcap, callcap;
	// can have pointers!
	noomV_Value* stack;
	noomV_CallFrame* calls;
	struct noomV_Thread* resumedBy;
	struct noomV_Thread* resuming;
} noomV_Thread;

struct noom_LuaVM {
	noomV_Object* heap;
	noomV_Object* graySet;
	noomV_Table* globals;
	noomV_Table* registry;
	noomV_Thread* mainThread;
	noomV_Thread* currentThread;
	noom_LuaVersion version;
};

// Allocating objects
noomV_Object* noomV_allocObj(noom_LuaVM* vm, noomV_ObjTag tag, noom_uint_t size);
noomV_String* noomV_allocStr(noom_LuaVM* vm, const char* str, noom_uint_t len);
noomV_Function* noomV_allocFunc(noom_LuaVM* vm, noomV_String* chunkname);
void noomV_freeObj(noomV_Object* obj);

#endif
