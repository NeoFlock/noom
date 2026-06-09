#include "noom.h"
#include "compiler.h"
#include "helper.h"

// COMPILATION!!!!!!!!!!!!!!!!!!!
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⢲⣞⣭⣟⣿⣻⣷⢶⢦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⣤⣞⣵⢯⣿⣞⣷⣿⣷⣿⣿⣿⣿⣜⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⣤⣶⢿⣾⣽⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣯⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⡤⢿⣿⣿⣿⣿⣯⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⢫⡀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⢆⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⡗⡇⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⢠⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣻⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⢰⣾⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡑⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠋⠉⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡷⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠻⠿⢿⣿⠟⠛⠛⠻⣿⣿⣿⣿⡿⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣇⠀⠀⠀⠉⢿⡟⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⠀⠀⠀⠀⣼⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠁⠀⠀⠀⢨⣿⡷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⠀⠀⠀⠀⣿⠿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⡀⠀⢀⠀⠀⠀⠀⠀⠀⠀⠀⢼⣿⣧⡴⢤⣶⡴⢻⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠘⠛⠿⠻⠷⠾⠶⠷⠾⠶⡷⣿⣿⣀⠀⠀⠀⠀⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⣀⠴⠮⠃⠉⠛⢷⣦⣠⡄⠀⣸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⣠⠶⠘⠁⠀⠀⠀⠀⠀⠙⢮⡿⣧⢾⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⣤⠓⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣈⣽⡿⢿⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⢀⣠⣠⣴⣮⠾⠟⠋⠉⢹⠌⠈⠙⠻⢷⣶⣤⣤⣀⡀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠙⠛⠛⠉⠁⠀⠀⠀⠀⠀⠀⡘⠂⠀⠀⠀⠀⠈⠉⠛⠛⠻⠿⠷⠦
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡘⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ 

void noomC_compiler_init(noomC_Compiler *compiler) {
	compiler->parent = 0;
	
	compiler->localc = 0;
	compiler->localcap = 0;
	compiler->locals = 0;
	
	compiler->constidx = 0;
	compiler->constc = 0;
	compiler->constcap = 0;
}

static noom_Exit noomC_emit(noomV_Function *func, const noomV_Inst inst) {
	func->code = noom_realloc(func->code, sizeof(noomV_Inst) * (func->codesize + 1));
	if (func->code == 0) return NOOM_ENOMEM;
	func->code[func->codesize++] = inst;
	return NOOM_OK;
}

static noom_Exit noomC_emit_ABC(noomV_Function *func, const noomV_Opcode op, const unsigned char a, const unsigned char b, const unsigned char c) {
	return noomC_emit(func, (noomV_Inst){.op = op, .a = a, .b = b, .c = c});
}

static noom_Exit noomC_emit_Aus(noomV_Function *func, const noomV_Opcode op, const unsigned char a, const unsigned short us) {
	return noomC_emit(func, (noomV_Inst){.op = op, .a = a, .us = us});
}

static noom_Exit noomC_addconst(noomV_Function *func, const noomV_Value val) {
	if (func->constsize == NOOM_USHORT_MAX) return NOOM_PLEASEHELPMEIAMSCARED;
	func->consts = noom_realloc(func->consts, sizeof(noomV_Value) * (func->constsize + 1));
	if (func->consts == 0) return NOOM_ENOMEM;
	func->consts[func->constsize++] = val;
	return NOOM_OK;
}

// TODO get rid of these two vvvv
static noom_Exit noomC_addconst_nil(noomV_Function *func) {
	return noomC_addconst(func, (noomV_Value){.tag = NOOMV_VNIL, .autoclose = 0, .isptr = 0});
}
static noom_Exit noomC_addconst_bool(noomV_Function *func, const noom_bool_t b) {
	return noomC_addconst(func, (noomV_Value){.tag = NOOMV_VBOOL, .autoclose = 0, .isptr = 0, .boolean =  b});
}
// yeah ^^^^^^^^^

static noom_Exit noomC_addconst_str(noomV_Function *func, noom_LuaVM *vm, const char *str, const noom_uint_t len) {
	noomV_String *s = noomV_allocStr(vm, str, len);
	if (s == 0) return NOOM_ENOMEM;
	return noomC_addconst(func, (noomV_Value){.tag = NOOMV_VOBJ, .autoclose = 0, .isptr = 0, .obj = (noomV_Object *)s});
}

static noomL_Token noomC_token_at(const noomP_Parser *parser, noom_uint_t offset) {
	noomL_Token token;
	noomL_lex(parser->code, offset, &token, parser->version);
	return token;
} 

// There is a high chance I forgot something here
static noomV_Opcode noomC_what_bop_is_this(const noomP_Parser *parser, noom_uint_t offset) {
	// I AM THE LEXER NOW!!!!!!!!!
	if (noom_startswith(parser->code + offset, "==")) return NOOMV_INSTR_EQ;
	if (noom_startswith(parser->code + offset, "~=")) return NOOMV_INSTR_NEQ;
	if (noom_startswith(parser->code + offset, "~=")) return NOOMV_INSTR_LTE;
	if (noom_startswith(parser->code + offset, "~=")) return NOOMV_INSTR_GTE;
	if (noom_startswith(parser->code + offset, "..")) return NOOMV_INSTR_CONCAT;
	
	if (parser->version >= NOOM_VERSION_53) {
		// Fucking atom forgot to put make an instruction
		// update no i am unable to read
		if (noom_startswith(parser->code + offset, "//")) return NOOMV_INSTR_FLOOR_DIV;
		if (noom_startswith(parser->code + offset, ">>")) return NOOMV_INSTR_RSHIFT;
		if (noom_startswith(parser->code + offset, "<<")) return NOOMV_INSTR_LSHIFT;
		if (noom_startswith(parser->code + offset, "&")) return NOOMV_INSTR_BAND;
		if (noom_startswith(parser->code + offset, "|")) return NOOMV_INSTR_BOR;
		if (noom_startswith(parser->code + offset, "~")) return NOOMV_INSTR_BXOR;
	}

	if (noom_startswith(parser->code + offset, "+")) return NOOMV_INSTR_ADD;
	if (noom_startswith(parser->code + offset, "-")) return NOOMV_INSTR_SUB;
	if (noom_startswith(parser->code + offset, "*")) return NOOMV_INSTR_MUL;
	if (noom_startswith(parser->code + offset, "/")) return NOOMV_INSTR_DIV;
	if (noom_startswith(parser->code + offset, "%")) return NOOMV_INSTR_MOD;
	if (noom_startswith(parser->code + offset, "^")) return NOOMV_INSTR_POW;
	if (noom_startswith(parser->code + offset, "<")) return NOOMV_INSTR_LT;
	if (noom_startswith(parser->code + offset, ">")) return NOOMV_INSTR_GT;
	return 0;
}

// mmap ahh function 🥀🥀🥀🥀🥀🥀🥀
// What 6 argument syscalls there even are other than mmap and clone and is there anything longer
static noom_Exit noomC_compile_expr(
		noom_LuaVM *vm,
		noomC_Compiler *compiler,
		const noomP_Parser *parser,
		noomV_Function *func,
		const noomP_Node *node
) {
	// Baba is You OST is a very cool soundtrack to code to Can recommend
	if (node->type == NOOMP_NODE_NILLITERAL) {
		unsigned char fucking_destination = compiler->curstack++;
		unsigned short fucking_constant = func->constsize;
		noom_Exit result = noomC_addconst_nil(func);
		if (result != NOOM_OK) return result;
		
		return noomC_emit_Aus(func, NOOMV_INSTR_LOADC, fucking_destination, fucking_constant);
	}
	if (node->type == NOOMP_NODE_BOOLEANLITERAL) {
		unsigned char fucking_destination = compiler->curstack++;
		unsigned short fucking_constant = func->constsize;
		noomL_Token bool_token = noomC_token_at(parser, node->source_offset);
		noom_bool_t val = noom_memeq(parser->code + bool_token.offset, bool_token.length, "true", 4);
		noom_Exit result = noomC_addconst_bool(func, val);
		if (result != NOOM_OK) return result;
		
		return noomC_emit_Aus(func, NOOMV_INSTR_LOADC, fucking_destination, fucking_constant);
	}
	if (node->type == NOOMP_NODE_NUMBERLITERAL) {
		unsigned char fucking_destination = compiler->curstack++;
		return NOOM_PLEASEHELPMEIAMSCARED; // 😭😭😭😭😭😭😭😭
	}
	if (node->type == NOOMP_NODE_STRINGLITERAL) {
		unsigned char fucking_destination = compiler->curstack++;
		noom_Exit result = noomC_addconst_str(func, vm, "but DID YOU KNOW", 16);
		return result;
	}
	if (node->type == NOOMP_NODE_BINARYOPERATOR) {
		unsigned char fucking_destination = compiler->curstack++;
		if (node->subnodec != 2) return NOOM_EINTERNAL;
		const unsigned char B = compiler->curstack++;
		noom_Exit result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0]);
		if (result != NOOM_OK) return result;
		const unsigned char C = compiler->curstack++;
		result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[1]);
		if (result != NOOM_OK) return result;
		
		return noomC_emit_ABC(func, noomC_what_bop_is_this(parser, node->source_offset), fucking_destination, B, C);
	}
	if (node->type == NOOMP_NODE_CALL) {
		unsigned char fucking_destination = compiler->curstack;
		// Here we don't fucking increment this fucking destination
		// Because the fucking function call fucking increments it fucking anyway
		for (int i = 1; i < node->subnodec; i++) {
			// push node->subnodes[i] to stack or something
		}
		return noomC_emit_Aus(func, NOOMV_INSTR_JMP, 0, /* where do i start......... */ 0);
	}
	if (node->type == NOOMP_NODE_METHODCALL) {
		unsigned char fucking_destination = compiler->curstack;
		// Here we don't fucking increment this fucking destination
		// Because the fucking function call fucking increments it fucking anyway
		// push parent value... probably already processed below if you make it i = 0
		for (int i = 1; i < node->subnodec; i++) {
			// push node->subnodes[i] to stack or something
		}
		return noomC_emit_Aus(func, NOOMV_INSTR_JMP, 0, /* where do i start......... */ 0);
	}
	if (node->type == NOOMP_NODE_GETFIELD) {
		unsigned char fucking_destination = compiler->curstack++;
		if (node->subnodec != 2) return NOOM_EINTERNAL;
		
		unsigned char B = compiler->curstack;
		noom_Exit result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0]);
		if (result != NOOM_OK) return result;
		
		unsigned char C = compiler->curstack;
		unsigned short C_constant = func->constsize;
		noomL_Token field_token = noomC_token_at(parser, node->subnodes[1]->source_offset);
		result = noomC_addconst_str(func, vm, parser->code + field_token.offset, field_token.length);
		if (result != NOOM_OK) return result;
		result = noomC_emit_Aus(func, NOOMV_INSTR_LOADC, C, C_constant);
		if (result != NOOM_OK) return result;
		
		return noomC_emit_ABC(func, NOOMV_INSTR_GET, fucking_destination, B, C);
	}
	return NOOM_EINTERNAL;
}

static noom_Exit noomC_add_stuff_to_function(noom_LuaVM *vm, noomC_Compiler *compiler, const noomP_Parser *parser, noomV_Function *func, const noomP_Node *node);

static noom_Exit noomC_compile_block(noom_LuaVM *vm, noomC_Compiler *compiler, const noomP_Parser *parser, noomV_Function *func, const noomP_Node *node) {
	noom_Exit r = NOOM_OK;
	for (noom_uint_t i = 0; i < node->subnodec; i++) {
		r = noomC_add_stuff_to_function(vm, compiler, parser, func, node->subnodes[i]);
		if (r != NOOM_OK) break;
	}
	return r;
}

// They say you use only 10% of your brain
// Keep it
// If you think too much you will get headaches

// I remember atom said there is a memory leak somewhere but I don't rememeememember where uhhhhhhhhhhhhhhhhh
static noom_Exit noomC_add_stuff_to_function(noom_LuaVM *vm, noomC_Compiler *compiler, const noomP_Parser *parser, noomV_Function *func, const noomP_Node *node) {
	// local name <attr> = expression i think
	if (node->type == NOOMP_NODE_LOCALDECLARATION) {
		if (node->subnodec != 1 && node->subnodec != 2) return NOOM_EINTERNAL;

		const noomP_Node *varname_node = node->subnodes[0];
		if (varname_node->type != NOOMP_NODE_ASSIGNPLACE) return NOOM_EINTERNAL;

		noomC_Local local;
		local.startpc = func->codesize;
		local.endpc = 0;
		local.dropped = 0; // FIXME?
		local.close = 0;
		local.constant = 0;
		local.name = noom_strndup(parser->code + varname_node->source_offset, varname_node->source_offset);

		for (noom_uint_t i = 0; i < varname_node->subnodec; i++) {
			if (varname_node->subnodes[i]->type != NOOMP_NODE_ATTRIBUTE) return NOOM_EINTERNAL;
			
			noomL_Token attr_token = noomC_token_at(parser, varname_node->subnodes[i]->source_offset);
			const char *ap = parser->code + attr_token.offset;
			noom_uint_t al = attr_token.length;
			if (noom_memeq(ap, al, "close", 5)) {
				if (local.close) return NOOM_EINTERNAL;
				local.close = 1;
			}
			else if (noom_memeq(ap, al, "const", 5)) {
				if (local.constant) return NOOM_EINTERNAL;
				local.constant = 1;
			}
			else {
				return NOOM_EINTERNAL;
			}
		}

		const unsigned destination = compiler->curstack;
		if (compiler->curstack++ == NOOM_MAXSTACK) return NOOM_ENOSTACK;

		if (node->subnodec == 1) {
			// according to atom this should be nil
			const unsigned short constptr = func->constsize;
			noom_Exit result = noomC_addconst_nil(func);
			if (result != NOOM_OK) return result;
			result = noomC_emit_Aus(func, NOOMV_INSTR_LOADC, destination, constptr);
			if (result != NOOM_OK) return result;
		}
		else {
			const noomP_Node *value_node = node->subnodes[1];
			const unsigned saved = compiler->curstack;
			compiler->curstack = destination;
			const noom_Exit r = noomC_compile_expr(vm, compiler, parser, func, value_node);
			if (r != NOOM_OK) return r;
			compiler->curstack = saved;
		}

		compiler->locals = noom_realloc(compiler->locals, sizeof(noomC_Local) * (compiler->localc + 1));
		if (compiler->locals == 0) return NOOM_ENOMEM;
		compiler->locals[compiler->localc++] = local;
		// I guess???????
		if (compiler->localc > compiler->localcap) compiler->localcap = compiler->localc;

		return NOOM_OK;
	}

	// local function name(...) ... end
	if (node->type == NOOMP_NODE_LOCALFUNCTIONDECLARATION) {
		if (node->subnodec != 3) return NOOM_EINTERNAL;
		// Funny staircase huh
		const noomP_Node *varname_node = node->subnodes[0];
		const noomP_Node *params_node = node->subnodes[1];
		const noomP_Node *block_node = node->subnodes[2];
		if (varname_node->type     != NOOMP_NODE_VARNAME) return NOOM_EINTERNAL;
		if (params_node->type     != NOOMP_NODE_FUNCTIONPARAMETERS) return NOOM_EINTERNAL;
		if (block_node->type     != NOOMP_NODE_BLOCK) return NOOM_EINTERNAL;

		const noomL_Token name_token = noomC_token_at(parser, varname_node->source_offset);
		noomV_Function *proto = noomV_allocFunc(vm, parser->code + name_token.offset, name_token.length);
		if (proto == 0) return NOOM_ENOMEM;

		proto->argc = (unsigned char)params_node->subnodec;

		noomC_Compiler child;
		noomC_compiler_init(&child);
		child.parent = compiler; // Is this correct????? Is this correct????????????????? It probably is
		child.curstack = proto->argc;

		const noom_Exit result = noomC_compile_block(vm, &child, parser, proto, block_node);
		if (result != NOOM_OK) return result;

		func->protos = noom_realloc(func->protos, sizeof(noomV_Function *) * (func->protosize + 1));
		if (func->protos == 0) return NOOM_ENOMEM;
		func->protos[func->protosize++] = proto;

		noomC_Local local;
		local.startpc = proto->codesize;
		local.endpc = 0;
		local.dropped = 0; // FIXME?
		local.close = 0;
		local.constant = 0;
		local.name = noom_strndup(parser->code + varname_node->source_offset, varname_node->source_offset);
		
		compiler->locals = noom_realloc(compiler->locals, sizeof(noomC_Local) * (compiler->localc + 1));
		if (compiler->locals == 0) return NOOM_ENOMEM;
		compiler->locals[compiler->localc++] = local;
		// I guess???????
		if (compiler->localc > compiler->localcap) compiler->localcap = compiler->localc;
		
		return NOOM_OK;
	}

	// TODO
	if (node->type == NOOMP_NODE_RETURN) {
		noomC_emit_Aus(func, NOOMV_INSTR_NOP, 0, 0);
		return NOOM_OK;
	}

	if (node->type == NOOMP_NODE_DOBLOCK || node->type == NOOMP_NODE_BLOCK) {
		return noomC_compile_block(vm, compiler, parser, func, node);
	}

	return NOOM_EINTERNAL;
}

noom_Exit noomC_compile(noom_LuaVM *vm, const noomP_Parser *parser, const noomP_Node *node) {
	if (node->type != NOOMP_NODE_PROGRAM) return NOOM_EINTERNAL;
	
	noomV_Function* program = noomV_allocFunc(vm, "main", 4);
	
	noomC_Compiler compiler;
	noomC_compiler_init(&compiler);
	
	noom_Exit status = noomC_compile_block(vm, &compiler, parser, program, node);
	if (status != NOOM_OK) return status;
	
	if (vm->mainThread->stacklen == vm->mainThread->stackcap) {
		vm->mainThread->stackcap = vm->mainThread->stackcap ? vm->mainThread->stackcap * 2 : 16;
		vm->mainThread->stack = noom_realloc(vm->mainThread->stack, sizeof(noomV_Value) * vm->mainThread->stackcap);
	}
	vm->mainThread->stack[vm->mainThread->stacklen++] =
		(noomV_Value){.tag = NOOMV_VOBJ, .autoclose = 0, .isptr = 0, .obj = (noomV_Object*)program};
	return NOOM_OK;
}
