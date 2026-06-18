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

typedef struct noomC_LocalInfo {
	enum { NOOMC_GLOBAL,
		   NOOMC_LOCAL,
		   NOOMC_UPVAL } type;
	unsigned int idx;
} noomC_LocalInfo;

noom_Exit noomC_addLocal(noomC_Compiler* c, noomC_Local local) {
	if (c->localc == NOOMC_MAXLOCAL) return NOOM_EINTERNAL;
	c->locals[c->localc++] = local;
	return NOOM_OK;
}

noom_Exit noomC_addUpval(noomC_Compiler* c, noomC_Upval upval) {
	if (c->upvalc == NOOMC_MAXUPVAL) return NOOM_EINTERNAL;
	c->upvals[c->upvalc++] = upval;
	return NOOM_OK;
}

noom_Exit noomC_identifyLocal(noomC_Compiler* compiler, noomC_LocalInfo* info, const char* name, noom_uint_t namelen) {
	// TODO: attempt to steal locals for upvalues

	for (int i = compiler->localc - 1; i >= 0; i--) {
		noomC_Local l = compiler->locals[i];
		if (l.dropped) continue;
		if (noom_memeq(l.name, l.namelen, name, namelen)) {
			info->type = NOOMC_LOCAL;
			info->idx = l.stackslot;
			return NOOM_OK;
		}
	}

	// fallback to global
	info->type = NOOMC_GLOBAL;
	return NOOM_OK;
}

void noomC_compiler_init(noomC_Compiler* compiler) {
	compiler->parent = 0;
	compiler->target = 0;

	compiler->localc = 0;
	compiler->upvalc = 0;
	compiler->curstack = 0;
}

static noom_Exit noomC_emit(noomV_Function* func, const noomV_Inst inst) {
	noomV_Inst* newCode = noom_realloc(func->code, sizeof(noomV_Inst) * (func->codesize + 1));
	if (newCode == 0) return NOOM_ENOMEM;
	func->code = newCode;
	func->code[func->codesize++] = inst;
	return NOOM_OK;
}

static noom_Exit noomC_emit_ABC(noomV_Function* func, const noomV_Opcode op, const unsigned char a, const unsigned char b, const unsigned char c) {
	return noomC_emit(func, (noomV_Inst){.op = op, .a = a, .b = b, .c = c});
}

static noom_Exit noomC_emit_Aus(noomV_Function* func, const noomV_Opcode op, const unsigned char a, const unsigned short us) {
	return noomC_emit(func, (noomV_Inst){.op = op, .a = a, .us = us});
}

static noom_Exit noomC_addconst(noomV_Function* func, const noomV_Value val) {
	if (func->constsize == NOOM_USHORT_MAX) return NOOM_PLEASEHELPMEIAMSCARED;
	noomV_Value* newConsts = noom_realloc(func->consts, sizeof(noomV_Value) * (func->constsize + 1));
	if (newConsts == 0) return NOOM_ENOMEM;
	func->consts = newConsts;
	func->consts[func->constsize++] = val;
	return NOOM_OK;
}

static noomV_String* noomC_internString(noomC_Compiler* c, noom_LuaVM* vm, const char* str, noom_uint_t len) {
	while (c) {
		noomV_Function* f = c->target;
		for (int i = 0; i < f->constsize; i++) {
			noomV_Value v = f->consts[i];
			if (v.tag != NOOMV_VOBJ) continue;
			noomV_Object* o = v.obj;
			if (o->tag != NOOMV_OSTR) continue;
			noomV_String* s = (noomV_String*)o;
			if (noom_memeq(s->data, s->len, str, len)) return s;
		}
		c = c->parent;
	}
	return noomV_allocStr(vm, str, len);
}

static noom_Exit noomC_addconst_str(noomC_Compiler* c, noom_LuaVM* vm, const char* str, const noom_uint_t len) {
	noomV_String* s = noomC_internString(c, vm, str, len);
	if (s == 0) return NOOM_ENOMEM;
	return noomC_addconst(c->target, (noomV_Value){.tag = NOOMV_VOBJ, .autoclose = 0, .isptr = 0, .obj = (noomV_Object*)s});
}

static noomL_Token noomC_token_at(const noomP_Parser* parser, noom_uint_t offset) {
	noomL_Token token;
	noomL_lex(parser->code, offset, &token, parser->version);
	return token;
}

// There is a high chance I forgot something here
static noom_BinOp noomC_what_bop_is_this(const noomP_Parser* parser, noom_uint_t offset) {
	const char* op = parser->code + offset;
	// I AM THE LEXER NOW!!!!!!!!!
	if (noom_startswith(op, "==")) return NOOM_BIN_EQL;
	if (noom_startswith(op, "~=")) return NOOM_BIN_NEQL;
	if (noom_startswith(op, "<")) return NOOM_BIN_LESS;
	if (noom_startswith(op, "<=")) return NOOM_BIN_LESSEQL;
	if (noom_startswith(op, ">")) return NOOM_BIN_GREATER;
	if (noom_startswith(op, ">=")) return NOOM_BIN_GREATEREQL;
	// no .., that is special cased due to funky behavior

	if (parser->version >= NOOM_VERSION_53) {
		// Fucking atom forgot to put make an instruction
		// update no i am unable to read
		if (noom_startswith(op, "//")) return NOOM_BIN_IDIV;
		if (noom_startswith(op, ">>")) return NOOM_BIN_BSHIFTR;
		if (noom_startswith(op, "<<")) return NOOM_BIN_BSHIFTL;
		if (noom_startswith(op, "&")) return NOOM_BIN_BAND;
		if (noom_startswith(op, "|")) return NOOM_BIN_BOR;
		if (noom_startswith(op, "~")) return NOOM_BIN_BXOR;
	}

	if (noom_startswith(op, "+")) return NOOM_BIN_ADD;
	if (noom_startswith(op, "-")) return NOOM_BIN_SUB;
	if (noom_startswith(op, "*")) return NOOM_BIN_MLT;
	if (noom_startswith(op, "/")) return NOOM_BIN_DIV;
	if (noom_startswith(op, "%")) return NOOM_BIN_MOD;
	if (noom_startswith(op, "^")) return NOOM_BIN_EXP;
	return 0;
}

// mmap ahh function 🥀🥀🥀🥀🥀🥀🥀
// What 6 argument syscalls there even are other than mmap and clone and is there anything longer
static noom_Exit noomC_compile_expr(
    noom_LuaVM* vm,
    noomC_Compiler* compiler,
    const noomP_Parser* parser,
    noomV_Function* func,
    const noomP_Node* node) {
	noom_Exit result;
	// Baba is You OST is a very cool soundtrack to code to Can recommend
	if (node->type == NOOMP_NODE_NILLITERAL) {
		compiler->curstack++;
		// TODO: analyze last instruction to optimize automatically
		return noomC_emit_Aus(func, NOOMV_INSTR_PUSHNIL, 0, 0);
	}
	if (node->type == NOOMP_NODE_BOOLEANLITERAL) {
		unsigned char fucking_destination = compiler->curstack++;
		unsigned short fucking_constant = func->constsize;
		noomL_Token bool_token = noomC_token_at(parser, node->source_offset);
		noom_bool_t val = noom_memeq(parser->code + bool_token.offset, bool_token.length, "true", 4);
		// TODO: analyze last instruction to optimize automatically
		return noomC_emit_Aus(func, NOOMV_INSTR_PUSHBOOLS, 0, val ? 1 : 0);
	}
	if (node->type == NOOMP_NODE_NUMBERLITERAL) {
		unsigned char fucking_destination = compiler->curstack++;
		return NOOM_PLEASEHELPMEIAMSCARED; // 😭😭😭😭😭😭😭😭
	}
	if (node->type == NOOMP_NODE_STRINGLITERAL) {
		unsigned char fucking_destination = compiler->curstack++;
		noom_Exit result = noomC_addconst_str(compiler, vm, "but DID YOU KNOW", 16);
		return result;
	}
	if (node->type == NOOMP_NODE_BINARYOPERATOR) {
		if (node->subnodec != 2) return NOOM_EINTERNAL;
		const char* op = parser->code + node->source_offset;
		if (noom_startswith(op, "..")) {
			// concat is special
			noom_uint_t amount = 1;
			const noomP_Node* n = node->subnodes[1];
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0]))) return result;
			// we have no flattening so we unwind it ourselves
			// unlike all left-associative ones, like +, where 1 + 2 + 3 is (1 + 2) + 3,
			// concat is right-associate, like ^, meaning 1 .. 2 .. 3 is 1 .. (2 .. 3).
			// VM will handle the concat order within the array for us
			while (1) {
				if (amount > NOOM_USHORT_MAX) return NOOM_EINTERNAL;
				amount++;
				if (noom_startswith(parser->code + n->source_offset, "..")) {
					if ((result = noomC_compile_expr(vm, compiler, parser, func, n->subnodes[0]))) return result;
					n = n->subnodes[1];
				} else {
					if ((result = noomC_compile_expr(vm, compiler, parser, func, n))) return result;
					break;
				}
			}
			compiler->curstack -= amount;
			compiler->curstack++;
			return noomC_emit_Aus(func, NOOMV_INSTR_CONCAT, 0, amount - 1);
		}

		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0]))) return result;
		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[1]))) return result;
		// this consumes 2 operands and pushes 1 value, thus having a net stack effect of removing 1 item
		compiler->curstack--;
		return noomC_emit_ABC(func, NOOMV_INSTR_OP, 1, noomC_what_bop_is_this(parser, node->source_offset), 0);
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
	if (node->type == NOOMP_NODE_VARIABLE) {
		noomC_LocalInfo info;
		const char* varname = parser->code + node->source_offset;
		noom_uint_t namelen = noomL_tokenlen(parser->code, node->source_offset, parser->version);
		if ((result = noomC_identifyLocal(compiler, &info, varname, namelen))) return result;
		compiler->curstack++;

		switch (info.type) {
			case NOOMC_LOCAL:
				return noomC_emit_Aus(func, NOOMV_INSTR_PUSHVAL, 0, info.idx);
			case NOOMC_UPVAL:
				return noomC_emit_Aus(func, NOOMV_INSTR_PUSHUPVAL, 0, info.idx);
			case NOOMC_GLOBAL:
				// FIXME: handle globals
				return NOOM_EINTERNAL;
		}
		// forgot a case
		return NOOM_EINTERNAL;
	}
	return NOOM_EINTERNAL;
}

static noom_Exit noomC_add_stuff_to_function(noom_LuaVM* vm, noomC_Compiler* compiler, const noomP_Parser* parser, noomV_Function* func, const noomP_Node* node);

static noom_Exit noomC_compile_block(noom_LuaVM* vm, noomC_Compiler* compiler, const noomP_Parser* parser, noomV_Function* func, const noomP_Node* node) {
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
static noom_Exit noomC_add_stuff_to_function(noom_LuaVM* vm, noomC_Compiler* compiler, const noomP_Parser* parser, noomV_Function* func, const noomP_Node* node) {
	noom_Exit result;
	// local name <attr> = expression i think
	if (node->type == NOOMP_NODE_LOCALDECLARATION) {
		// FIXME: there can be multiple locals defined
		// This code is awful for several reasons and we shall burn it
		if (node->subnodec != 1 && node->subnodec != 2) return NOOM_EINTERNAL;

		const noomP_Node* varname_node = node->subnodes[0];
		if (varname_node->type != NOOMP_NODE_ASSIGNPLACE) return NOOM_EINTERNAL;

		const char* namebuf = parser->code + varname_node->source_offset;

		noomC_Local local;
		local.startpc = func->codesize;
		local.endpc = 0;
		local.dropped = 0;
		local.close = 0;
		local.constant = 0;
		local.name = namebuf;
		local.namelen = noomL_tokenlen(namebuf, 0, parser->version);

		for (noom_uint_t i = 0; i < varname_node->subnodec; i++) {
			if (varname_node->subnodes[i]->type != NOOMP_NODE_ATTRIBUTE) return NOOM_EINTERNAL;

			noomL_Token attr_token = noomC_token_at(parser, varname_node->subnodes[i]->source_offset);
			const char* ap = parser->code + attr_token.offset;
			noom_uint_t al = attr_token.length;
			if (noom_memeq(ap, al, "close", 5)) {
				if (local.close) return NOOM_EINTERNAL;
				local.close = 1;
			} else if (noom_memeq(ap, al, "const", 5)) {
				if (local.constant) return NOOM_EINTERNAL;
				local.constant = 1;
			} else {
				return NOOM_EINTERNAL;
			}
		}

		const unsigned destination = compiler->curstack;
		if (compiler->curstack++ == NOOM_MAXSTACK) return NOOM_ENOSTACK;

		if (node->subnodec == 1) {
			const unsigned short constptr = func->constsize;
			result = noomC_emit_Aus(func, NOOMV_INSTR_PUSHNIL, 0, 0);
			if (result != NOOM_OK) return result;
			compiler->curstack++;
		} else {
			const noomP_Node* value_node = node->subnodes[1];
			const noom_Exit r = noomC_compile_expr(vm, compiler, parser, func, value_node);
			if (r != NOOM_OK) return r;
		}

		local.stackslot = compiler->curstack;

		return noomC_addLocal(compiler, local);
	}

	// local function name(...) ... end
	if (node->type == NOOMP_NODE_LOCALFUNCTIONDECLARATION) {
		if (node->subnodec != 3) return NOOM_EINTERNAL;
		// Funny staircase huh
		const noomP_Node* varname_node = node->subnodes[0];
		const noomP_Node* params_node = node->subnodes[1];
		const noomP_Node* block_node = node->subnodes[2];
		if (varname_node->type != NOOMP_NODE_VARNAME) return NOOM_EINTERNAL;
		if (params_node->type != NOOMP_NODE_FUNCTIONPARAMETERS) return NOOM_EINTERNAL;
		if (block_node->type != NOOMP_NODE_BLOCK) return NOOM_EINTERNAL;

		const noomL_Token name_token = noomC_token_at(parser, varname_node->source_offset);
		noomV_Function* proto = noomV_allocFunc(vm, func->chunkname);
		if (proto == 0) return NOOM_ENOMEM;

		proto->argc = (unsigned char)params_node->subnodec;

		noomC_Compiler child;
		noomC_compiler_init(&child);
		child.parent = compiler; // Is this correct????? Is this correct????????????????? It probably is
		child.curstack = proto->argc;

		const noom_Exit result = noomC_compile_block(vm, &child, parser, proto, block_node);
		if (result != NOOM_OK) return result;

		noomV_Function** newProtos = noom_realloc(func->protos, sizeof(noomV_Function*) * (func->protosize + 1));
		if (newProtos == 0) return NOOM_ENOMEM;
		func->protos = newProtos;
		func->protos[func->protosize++] = proto;

		noomC_Local local;
		local.startpc = proto->codesize;
		local.endpc = 0;
		local.dropped = 0;
		local.close = 0;
		local.constant = 0;
		local.name = parser->code + varname_node->source_offset;
		local.namelen = noomL_tokenlen(parser->code, varname_node->source_offset, parser->version);

		return noomC_addLocal(compiler, local);
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

noom_Exit noomC_compile(noom_LuaVM* vm, const noomP_Parser* parser, const noomP_Node* node, noomV_String* chunkname, noomV_Table* env) {
	if (node->type != NOOMP_NODE_PROGRAM) return NOOM_EINTERNAL;

	noomV_Function* program = noomV_allocFunc(vm, chunkname);
	if (program == 0) return NOOM_ENOMEM;

	noomC_Compiler compiler;
	noomC_compiler_init(&compiler);
	compiler.target = program;

	if (parser->version == NOOM_VERSION_51) {
		program->env = env;
	} else {
		noomC_Upval upval;
		upval.name = "_ENV";
		upval.namelen = 4;
		upval.slot = 0;
		upval.stolen = 1;
		upval.constant = 0;
		noomC_addUpval(&compiler, upval);
	}

	noom_Exit status = noomC_compile_block(vm, &compiler, parser, program, node);
	if (status != NOOM_OK) return status;

	if (parser->version > NOOM_VERSION_51) {
		// TODO: we need to wrap the value as a *closure*, with one upvalue, the environment (_ENV).
	}

	if (vm->mainThread->stacklen == vm->mainThread->stackcap) {
		vm->mainThread->stackcap = vm->mainThread->stackcap ? vm->mainThread->stackcap * 2 : 16;
		vm->mainThread->stack = noom_realloc(vm->mainThread->stack, sizeof(noomV_Value) * vm->mainThread->stackcap);
	}
	vm->mainThread->stack[vm->mainThread->stacklen++] =
	    (noomV_Value){.tag = NOOMV_VOBJ, .autoclose = 0, .isptr = 0, .obj = (noomV_Object*)program};
	return NOOM_OK;
}
