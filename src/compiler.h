#include "noom.h"
#include "vm.h"
#include "parser.h"

// I am sorry

typedef struct noomC_Local {
	unsigned int startpc;
	unsigned int endpc;
	const char* name;
	unsigned int namelen;
	unsigned int stackslot;
	bool dropped;
	bool constant;
	bool close;
} noomC_Local;

typedef struct noomC_Upval {
	const char* name;
	unsigned int namelen;
	unsigned short slot;
	// if stolen, this means that slot is actually an upvalue index
	bool isParentUpvalue;
	bool constant;
} noomC_Upval;

#define NOOMC_MAXLOCAL 200
#define NOOMC_MAXUPVAL 64

#define NOOMC_MAXBREAKS 64
#define NOOMC_MAXLOOPDEPTH 32

#define NOOMC_MAXLABELS 32
#define NOOMC_MAXGOTOS 32

typedef struct noomC_Label {
	const char* name;
	noom_uint_t namelen;
	unsigned int pc;
} noomC_Label;

typedef struct noomC_PendingGoto {
	const char* name;
	noom_uint_t namelen;
	unsigned int patchpc;
} noomC_PendingGoto;

typedef struct noomC_Compiler {
	// steal constants from this
	struct noomC_Compiler* parent;
	noomV_Function* target;
	char *stringTmpBuf;
	noom_uint_t maxStringLen;
	unsigned localc;
	unsigned upvalc;
	unsigned curstack;
	noomC_Local locals[NOOMC_MAXLOCAL];
	noomC_Upval upvals[NOOMC_MAXUPVAL];
	
	noomC_Label labels[NOOMC_MAXLABELS];
	unsigned int labelc;
	noomC_PendingGoto pendingGotos[NOOMC_MAXGOTOS];
	unsigned int pendingGotoc;
	
	unsigned int breakPatches[NOOMC_MAXBREAKS];
	unsigned int breakPatchCount;
	unsigned int loopBreakBase[NOOMC_MAXLOOPDEPTH];
	unsigned int loopDepth;
} noomC_Compiler;

void noomC_compiler_init(noomC_Compiler* compiler);
void noomC_compiler_deinit(noomC_Compiler* compiler);

noom_Exit noomC_emit(noomV_Function* func, noomV_Inst inst);
#define noomC_emit_ABC(func, _op, _a, _b, _c) noomC_emit((func), (noomV_Inst){.op = (_op), .a = (_a), .b = (_b), .c = (_c)})
#define noomC_emit_AuD(func, _op, _a, _uD) noomC_emit((func), (noomV_Inst){.op = (_op), .a = (_a), .us = (_uD)})
noom_BinOp noomC_lex_bin_op(const noomP_Parser* parser, noom_uint_t offset);
noom_UnaryOp noomC_lex_un_op(const noomP_Parser* parser, noom_uint_t offset);

static noom_Exit noomC_compile_proto(
	noom_LuaVM* vm,
	noomC_Compiler* parent_compiler,
	const noomP_Parser* parser,
	noomV_Function* parent_func,
	const noomP_Node* params_node,
	const noomP_Node* block_node,
	bool has_self,
	noomV_Function** out_proto);
noom_Exit noomC_emit_assign_to(
	noom_LuaVM* vm,
	noomC_Compiler* compiler,
	const noomP_Parser* parser,
	noomV_Function* func,
	const noomP_Node* target,
	unsigned char value_slot);
noom_Exit noomC_compile_expr(
	noom_LuaVM* vm,
	noomC_Compiler* compiler,
	const noomP_Parser* parser,
	noomV_Function* func,
	const noomP_Node* node,
	// retc of -1 means all values!!!!!!!!!!!
	int retc);
noom_Exit noomC_compile_block(noom_LuaVM* vm, noomC_Compiler* compiler, const noomP_Parser* parser, noomV_Function* func, const noomP_Node* node);
noom_Exit noomC_add_stuff_to_function(noom_LuaVM* vm, noomC_Compiler* compiler, const noomP_Parser* parser, noomV_Function* func, const noomP_Node* node);

// pushes the compiled function on the stack, or just crashes lol
noom_Exit noomC_compile(noom_LuaVM* vm, const noomP_Parser* parser, const noomP_Node* node, noomV_String* chunkname, noomV_Table* env, noomV_Value* outFunc);
