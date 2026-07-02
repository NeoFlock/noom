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

// TODO: probably many of these don't really need to be exposed to other code but whatever
noom_Exit noomC_addLocal(noomC_Compiler* c, noomC_Local local);
noom_Exit noomC_addUpval(noomC_Compiler* c, noomC_Upval upval);
int noomC_stealUpval(noomC_Compiler* c, const char* name, noom_uint_t namelen);

void noomC_compiler_init(noomC_Compiler* compiler);

noom_Exit noomC_emit(noomV_Function* func, noomV_Inst inst);
#define noomC_emit_ABC(func, _op, _a, _b, _c) noomC_emit((func), (noomV_Inst){.op = (_op), .a = (_a), .b = (_b), .c = (_c)})
#define noomC_emit_AuD(func, _op, _a, _uD) noomC_emit((func), (noomV_Inst){.op = (_op), .a = (_a), .us = (_uD)})
noom_Exit noomC_addconst(noomV_Function* func, noomV_Value val);
noomV_String* noomC_internString(const noomC_Compiler* c, noom_LuaVM* vm, const char* str, noom_uint_t len);
noom_Exit noomC_addconst_str(noomC_Compiler* c, noom_LuaVM* vm, const char* str, noom_uint_t len);
noom_BinOp noomC_lex_bin_op(const noomP_Parser* parser, noom_uint_t offset);
noom_UnaryOp noomC_lex_un_op(const noomP_Parser* parser, noom_uint_t offset);

static noom_Exit noomC_compile_proto(
	noom_LuaVM* vm,
	noomC_Compiler* parent_compiler,
	const noomP_Parser* parser,
	noomV_Function* parent_func,
	const noomP_Node* params_node,
	const noomP_Node* block_node,
	noom_bool_t has_self,
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
