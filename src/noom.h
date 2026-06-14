#ifndef NOOM_H
#define NOOM_H

#define NN_STR(x) #x
#define NN_XSTR(x) NN_STR(x)

#define NOOM_VERSION_MAJOR 0
#define NOOM_VERSION_MINOR 0
#define NOOM_VERSION_PATCH 0
#if NOOM_VERSION_PATCH==0
#define NOOM_VERSION_FULL NN_XSTR(NOOM_VERSION_MAJOR) "." NN_XSTR(NOOM_VERSION_MINOR)
#else
#define NOOM_VERSION_FULL NN_XSTR(NOOM_VERSION_MAJOR) "." NN_XSTR(NOOM_VERSION_MINOR) "." NN_XSTR(NOOM_VERSION_PATCH)
#endif
#define NOOM_VERSION_TEXT "Noom " NOOM_VERSION_FULL "  (C) 2026 NeoFlock and Noom contributors"

#ifdef __cplusplus
extern "C" {
#endif

// VM stackspace guaranteed to exist when calling C functions
// If C pushes less than this, it is guaranteed to not stackoverflow
#define NOOM_MINSTACK 32

// Maximum amount of values the stack can hold
// To prevent recursion from using up infinitely much memory
#define NOOM_MAXSTACK 16384

typedef unsigned long long int noom_uint_t;
typedef signed long long int noom_int_t;
typedef double noom_float_t;
typedef unsigned char noom_bool_t;

typedef enum noom_LuaVersion {
	// no 5.0, at least for now, cause it doesn't seem to be used much and is a bit *weird*
	NOOM_VERSION_51,
	NOOM_VERSION_52,
	NOOM_VERSION_53,
	NOOM_VERSION_54
	// no you don't get 5.5; if someone wants to do it and maintain it, they can, i'm not gonna.
} noom_LuaVersion;

typedef enum noom_LuaType {
	NOOM_LUA_TNIL,
	NOOM_LUA_TBOOL,
	NOOM_LUA_TNUM,
	NOOM_LUA_TSTR,
	NOOM_LUA_TTABLE,
	NOOM_LUA_TFUNCTION,
	NOOM_LUA_TTHREAD,
	NOOM_LUA_TUSERDATA,
} noom_LuaType;

typedef enum noom_Exit {
	NOOM_OK = 0,
	NOOM_PLEASEHELPMEIAMSCARED,
	NOOM_IAMNOTSCAREDJUSTLAZYTHISTIME,
	// unknown internal error
	NOOM_EINTERNAL,
	// out of memory
	NOOM_ENOMEM,
	// runtime error
	NOOM_ERUNTIME,
	// stack overflow
	NOOM_ENOSTACK,
	// I/O error
	NOOM_EIO,
	// buffer overflow
	NOOM_ELIMIT,
	// error in error handler
	NOOM_EERROR,
} noom_Exit;

typedef enum noom_UnaryOp {
	// -x
	NOOM_UNARY_NEGATE,
	// ~x
	NOOM_UNARY_BNOT,
	// #x
	NOOM_UNARY_LEN,
	// not x
	NOOM_UNARY_NOT,
} noom_UnaryOp;

typedef enum noom_BinOp {
	// Algebruh (Noom knows maths)

	// a + b
	NOOM_BIN_ADD,
	// a - b
	NOOM_BIN_SUB,
	// a * b
	NOOM_BIN_MLT,
	// a / b
	NOOM_BIN_DIV,
	// a // b
	NOOM_BIN_IDIV,
	// a ^ b
	NOOM_BIN_EXP,

	// Comparison (thief of joy)

	// a == b
	NOOM_BIN_EQL,
	// a ~= b
	NOOM_BIN_NEQL,
	// a < b
	NOOM_BIN_LESS,
	// a <= b
	NOOM_BIN_LESSEQL,
	// a > b
	NOOM_BIN_GREATER,
	// a >= b
	NOOM_BIN_GREATEREQL,

	// a & b
	NOOM_BIN_BAND,
	// a | b
	NOOM_BIN_BOR,
	// a ~ b
	NOOM_BIN_BXOR,
	// a << b
	NOOM_BIN_BSHIFTL,
	// a >> b
	NOOM_BIN_BSHIFTR,
} noom_BinOp;

typedef enum noom_CoroutineStatus {
	// Coroutine is the running one
	NOOM_CORO_RUNNING,
	// Yielded
	NOOM_CORO_SUSPENDED,
	// Resuming another coroutine
	NOOM_CORO_NORMAL,
	// Done
	NOOM_CORO_DEAD,
} noom_CoroutineStatus;

typedef struct noom_LuaVM noom_LuaVM;

typedef noom_Exit noom_CFunction(noom_LuaVM *vm);
typedef noom_Exit noom_KFunction(noom_LuaVM *vm, noom_Exit status, void *ctx);

noom_LuaVM *noom_createVM(noom_LuaVersion version);
void noom_destroyVM(noom_LuaVM *vm);

// type of a stack slot.
// If negative, its relative to the top where -1 is the top.
// 0 is the first stack value.
// Do note that it is relative to the current stackframe.
typedef noom_int_t noom_slot_t;

// Push data

noom_Exit noom_pushint(noom_LuaVM *vm, noom_int_t integer);
noom_Exit noom_pushnumber(noom_LuaVM *vm, noom_float_t number);
noom_Exit noom_pushbool(noom_LuaVM *vm, noom_bool_t boolean);
noom_Exit noom_pushstring(noom_LuaVM *vm, const char *s);
noom_Exit noom_pushlstring(noom_LuaVM *vm, const char *s, noom_uint_t len);
noom_Exit noom_createtable(noom_LuaVM *vm, noom_uint_t prealloc);
// Push a C closure using the top [upvalues] stackslots as upvalues
noom_Exit noom_pushcclosure(noom_LuaVM *vm, noom_CFunction *f, noom_uint_t upvalues);
#define noom_pushcfunction(vm, f) noom_pushcclosure((vm), (f), 0)
noom_Exit noom_pushuserdata(noom_LuaVM *vm, void *userdata);
noom_Exit noom_pushlightuserdata(noom_LuaVM *vm, void *userdata);
// Pops the top function and pushes a new coroutine with that function being called
noom_Exit noom_pushcoroutine(noom_LuaVM *vm);
// Pushes the error object, if any. If no error, nil is pushed
noom_Exit noom_pusherror(noom_LuaVM *vm);
// Pushes the globals table itself
noom_Exit noom_pushglobals(noom_LuaVM *vm);
// Pushes the registry itself
noom_Exit noom_pushregistry(noom_LuaVM *vm);
// Pushes the running coroutine
noom_Exit noom_pushrunning(noom_LuaVM *vm);

// Stack stuff

// Gets the amount of values on the stack
// Signed because of C math rules, but it cannot be negative and is perfectly safe to cast to unsigned
noom_int_t noom_getstacksize(noom_LuaVM *vm);
// Sets the stack size, popping or pushing nil as need be.
// Its signed so it gives a stack *underflow* on negative values, instead of a confusing stack overflow
noom_Exit noom_setstacksize(noom_LuaVM *vm, noom_int_t amount);
#define noom_pushnils(vm, x) noom_setstacksize((vm), noom_getstacksize((vm))+(x))
#define noom_pushnil(vm) noom_pushnils((vm), 1)
#define noom_popn(vm, n) noom_setstacksize((vm), noom_getstacksize((vm)) - (n))
#define noom_pop(vm) noom_popn((vm), 1)
// Marks a stackslot as to-be-closed
noom_Exit noom_markTBC(noom_LuaVM *vm, noom_slot_t slot);
// Preallocate stack space to OOM/stack overflow *early*
noom_Exit noom_preallocStack(noom_LuaVM *vm, noom_uint_t amount);
// Set the value at [dst] to [src]
noom_Exit noom_copyto(noom_LuaVM *vm, noom_slot_t dst, noom_slot_t src);
// Set the value at [dst] to be a pointer to [src].
// This is used to link upvalues.
// This means that writes to either are reflected in both.
noom_Exit noom_linkto(noom_LuaVM *vm, noom_slot_t dst, noom_slot_t src);
// Stack rotation
noom_Exit noom_rotate(noom_LuaVM *vm, noom_int_t amount);

// Operations

// Dupes the top [n] values [m] times
noom_Exit noom_dupearr(noom_LuaVM *vm, noom_uint_t n, noom_uint_t m);
#define noom_dupen(vm, n) noom_dupearr((vm), 1, (n))
#define noom_dupe(vm) noom_dupen((vm), 1)

// Calls a function with a given continuation function in case of yield.
// If f's memory address is 0, the call CANNOT yield, and any attempts to do so are an error.
// If this call cannot yield, it is fine to run code after it returns. If it can yield, DO NOT, as even if the call does not yield,
// the continuation function is invoked, and thus you should return it directly.
// This will pop argc+1 values, where the first one, at [-argc-1], is the value being called, and everything after is arguments.
// [retc] is the desired return count. It is signed, as negative values mean to return ALL values, regardless of how many.
// If the function returns too many values, the excess is popped. If it returns too few, nils are pushed.
noom_Exit noom_callk(noom_LuaVM *vm, noom_uint_t argc, noom_int_t retc, noom_KFunction *f, void *ctx);
#define noom_call(vm, argc, retc) noom_callk((vm), (argc), (retc), 0, 0)
// Sets the return count of the function. Should be the last statement to prevent confusing behavior.
// The actual *values* returned are the top [retc] values on the stack after the function exits.
noom_Exit noom_return(noom_LuaVM *vm, noom_uint_t retc);
// Attempts to yield the current coroutine with the top [n] values and a supplied continuation function.
// If the continuation function's memory address is 0, it is replaced with a default function which
// does nothing and returns no values.
// When the coroutine is resumed, the continuation function is invoked.
noom_Exit noom_yieldk(noom_LuaVM *vm, noom_uint_t n, noom_KFunction *f, void *ctx);
#define noom_yield(vm, n) noom_yieldk((vm), (n), 0, 0)

// Like a noom_call, except instead of calling, it resumes coroutines.
// This cannot yield but allows yields.
noom_Exit noom_resume(noom_LuaVM *vm, noom_uint_t argc, noom_uint_t retc);
// Pops a coroutine, gives its status through the out-pointer
noom_Exit noom_corostatus(noom_LuaVM *vm, noom_CoroutineStatus *status);
// Pops a coroutine, closes it.
// This means it'll unwind the callstack of the coroutine and call to-be-closed values as needed.
// If those error, which can happen, this errors as well.
noom_Exit noom_coroclose(noom_LuaVM *vm);

// Get the type of a slot. Invalid slots are just [nil]
noom_LuaType noom_typeof(noom_LuaVM *vm, noom_slot_t x);

// TODO: evaluate if k variants of these should be used, as
// these *could* yield due to metatables

// Pops the top 2 values and pushes the result of the operation
noom_Exit noom_binop(noom_LuaVM *vm, noom_BinOp op);
// Pops the top 2 values and pushes the result of the operation
noom_Exit noom_unaryop(noom_LuaVM *vm, noom_UnaryOp op);

// Pops (table, key) and pushes `table[key]`
noom_Exit noom_gettable(noom_LuaVM *vm);
// Pops (table, key, value) and sets `table[key] = value`
noom_Exit noom_settable(noom_LuaVM *vm);
// Gets the metatable. This gets the actual metatable, __metatable is ignored
noom_Exit noom_getmetatable(noom_LuaVM *vm);
// Pops (table, metatable) and sets the metatable of [table] to [metatable]
noom_Exit noom_setmetatable(noom_LuaVM *vm);

// Given stack values (table, key), it pops key and pushes the next key as well as `table[nkey]`.
// If key is nil, the next key is actually the first key of the table, as nil cannot be a key.
// This lets you iterate stuff with like `pairs`.
// This DOES NOT account for __pairs.
noom_Exit noom_next(noom_LuaVM *vm);

// Taking data out

noom_Exit noom_tobool(noom_LuaVM *vm, noom_slot_t x, noom_bool_t *b);
noom_Exit noom_toint(noom_LuaVM *vm, noom_slot_t x, noom_int_t *n);
noom_Exit noom_tonumber(noom_LuaVM *vm, noom_slot_t x, noom_float_t *n);
// NOTE: the string is not automatically retained until function exit.
// KEEP THE VALUE SOMEWHERE OR GC MAY FREE IT.
noom_Exit noom_tolstring(noom_LuaVM *vm, noom_slot_t x, const char **s, noom_uint_t *len);
#define noom_tostring(vm, x, s) noom_tolstring((vm), (x), (s), 0)
// Casts to a pointer if applicable, memory address 0x0 if not.
// The pointer is effectively meaningless, and only really matters
// for the %p format specifier in `string.format()`
noom_Exit noom_topointer(noom_LuaVM *vm, noom_slot_t x, const void **p);

// Type coercion

// Converts the value at [x] to a boolean
noom_Exit noom_cast2truthy(noom_LuaVM *vm, noom_slot_t x);
// Converts the value at [x] to a number
noom_Exit noom_cast2num(noom_LuaVM *vm, noom_slot_t x);
// Converts the value at [x] to a string, ignoring __tostring and __name.
noom_Exit noom_cast2str(noom_LuaVM *vm, noom_slot_t x);

// Misc

// Run the garbage collector to clean up memory.
// DO NOT RUN IN THE ALLOCATOR IF ANY IS SUPPLIED, OR YOU WILL GET THE WORST POSSIBLE ERRORS.
void noom_gc(noom_LuaVM *vm);

#ifdef __cplusplus
}
#endif

#endif
