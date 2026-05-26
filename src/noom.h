// #define NOOM_USE_NFT

#ifndef NOOM_H
#define NOOM_H


#define STR(x) #x
#define XSTR(x) STR(x)

#define NOOM_VERSION_MAJOR 0
#define NOOM_VERSION_MINOR 0
#define NOOM_VERSION_PATCH 0
#if NOOM_VERSION_PATCH==0
#define NOOM_VERSION_FULL XSTR(NOOM_VERSION_MAJOR) "." XSTR(NOOM_VERSION_MINOR)
#else
#define NOOM_VERSION_FULL XSTR(NOOM_VERSION_MAJOR) "." XSTR(NOOM_VERSION_MINOR) "." XSTR(NOOM_VERSION_PATCH)
#endif
#define NOOM_VERSION_TEXT "Noom " NOOM_VERSION_FULL "  (C) 2026 NeoFlock and Noom contributors"

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
	NOOM_YIELD,
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

typedef struct noom_LuaVM noom_LuaVM;

typedef noom_Exit noom_CFunction(noom_LuaVM *vm);
typedef noom_Exit noom_KFunction(noom_LuaVM *vm, noom_Exit status, void *ctx);

#endif
