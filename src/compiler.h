#include "noom.h"
#include "vm.h"
#include "parser.h"

typedef struct noomC_Local {
	unsigned int startpc;
	unsigned int endpc;
	const char* name;
	unsigned int namelen;
	unsigned int stackslot;
	noom_bool_t dropped;
	noom_bool_t constant;
	noom_bool_t close;
} noomC_Local;

typedef struct noomC_Upval {
	const char* name;
	unsigned int namelen;
	unsigned short slot;
	// if stolen, this means that slot is actually an upvalue index
	noom_bool_t stolen;
	noom_bool_t constant;
} noomC_Upval;

#define NOOMC_MAXLOCAL 200
#define NOOMC_MAXUPVAL 64

typedef struct noomC_Compiler {
	// steal constants from this
	struct noomC_Compiler* parent;
	noomV_Function* target;
	unsigned localc;
	unsigned upvalc;
	unsigned curstack;
	noomC_Local locals[NOOMC_MAXLOCAL];
	noomC_Upval upvals[NOOMC_MAXUPVAL];
} noomC_Compiler;

void noomC_compiler_init(noomC_Compiler* compiler);

// pushes the compiled function on the stack, or just crashes lol
noom_Exit noomC_compile(noom_LuaVM* vm, const noomP_Parser* parser, const noomP_Node* node, noomV_String* chunkname, noomV_Table* env, noomV_Value* outFunc);
