#include "noom.h"
#include "vm.h"
#include "parser.h"

typedef struct noomC_Local {
	unsigned int startpc;
	unsigned int endpc;
	const char *name;
	unsigned int namelen;
	unsigned int stackslot;
	noom_bool_t dropped;
	noom_bool_t constant;
	noom_bool_t close;
} noomC_Local;

typedef struct noomC_Compiler {
	// steal constants from this
	struct noomC_Compiler *parent;
	unsigned localc, constc;
	unsigned localcap, constcap;
	// constants index in the stack.
	// We put it there so GC doesn't free them, and
	// so we can allocate the function's consts arrays *ONCE*
	unsigned constidx;
	unsigned curstack;
	noomC_Local *locals;
} noomC_Compiler;

void noomC_compiler_init(noomC_Compiler* compiler);

// pushes the compiled function on the stack, or just crashes lol
noom_Exit noomC_compile(noom_LuaVM *vm, const noomP_Parser *parser, const noomP_Node *node);
