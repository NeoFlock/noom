#include "types.h"
#include "vm.h"
#include "parser.h"

typedef struct noomC_Local {
	unsigned int startpc;
	unsigned int endpc;
	char *name;
	bool dropped;
	bool constant;
	bool close;
} noomC_Local;

typedef struct noomC_Compiler {
	// steal constants from this
	struct noomC_Compiler *parent;
	unsigned localc, constc;
	unsigned localcap, constcap;
	// constants index in the stack.
	// We put it there so GC doesn't free them.
	unsigned constidx;
	unsigned curstack;
	noomC_Local *locals;
} noomC_Compiler;

// pushes the compiled function on the stack, or just crashes lol
noom_Exit noomC_compile(noom_LuaVM *vm, noomP_Node *node);
