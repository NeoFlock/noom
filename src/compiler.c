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

int noomC_stealUpval(noomC_Compiler* c, const char* name, noom_uint_t namelen) {
	// hold on hold on hold on but what if...
	for (unsigned i = 0; i < c->upvalc; i++) {
		const noomC_Upval u = c->upvals[i];
		if (noom_memeq(u.name, u.namelen, name, namelen)) return (int)i;
	}
	// damn it

	if (c->parent == 0) return -1;

	// maybe you have smth useful
	for (int i = (int)c->parent->localc - 1; i >= 0; i--) {
		const noomC_Local l = c->parent->locals[i];
		if (l.dropped) continue;
		if (noom_memeq(l.name, l.namelen, name, namelen)) {
			// yay!!!! wahoo!!!! yippee!!!!!!!!!!!
			if (c->upvalc == NOOMC_MAXUPVAL) return -1;
			noomC_Upval upval;
			upval.name = name;
			upval.namelen = namelen;
			upval.slot = (unsigned short)l.stackslot;
			upval.stolen = 0;
			upval.constant = l.constant;
			const unsigned idx = c->upvalc;
			c->upvals[c->upvalc++] = upval;
			return (int)idx;
		}
	}

	const int parent_upval = noomC_stealUpval(c->parent, name, namelen);
	if (parent_upval >= 0) {
		if (c->upvalc == NOOMC_MAXUPVAL) return -1;
		noomC_Upval upval;
		upval.name = name;
		upval.namelen = namelen;
		upval.slot = (unsigned short)parent_upval;
		upval.stolen = 1; 
		upval.constant = c->parent->upvals[parent_upval].constant;
		const unsigned idx = c->upvalc;
		c->upvals[c->upvalc++] = upval;
		return (int)idx;
	}

	return -1;
}

static noomC_LocalInfo noomC_identifyLocal(noomC_Compiler* compiler, const char* name, noom_uint_t namelen) {
	for (int i = compiler->localc - 1; i >= 0; i--) {
		const noomC_Local l = compiler->locals[i];
		if (l.dropped) continue;
		if (noom_memeq(l.name, l.namelen, name, namelen)) {
			return (noomC_LocalInfo){ .type = NOOMC_LOCAL, .idx = l.stackslot };
		}
	}

	const int upval_idx = noomC_stealUpval(compiler, name, namelen);
	if (upval_idx >= 0) {
		return (noomC_LocalInfo){ .type = NOOMC_UPVAL, .idx = (unsigned)upval_idx };
	}

	return (noomC_LocalInfo){ .type = NOOMC_GLOBAL };
}

static noom_Exit noomC_identifyLocalAndSet(noomC_Compiler* compiler, const char* name, noom_uint_t namelen) {
	const noomC_LocalInfo info = noomC_identifyLocal(compiler, name, namelen);
	if (info.type == NOOMC_LOCAL) return noomC_emit_AuD(compiler->target, NOOMV_INSTR_SETVAL, compiler->curstack++, (unsigned short)info.idx);
	if (info.type == NOOMC_UPVAL) return noomC_emit_AuD(compiler->target, NOOMV_INSTR_SETUPVAL, compiler->curstack++, (unsigned short)info.idx);
	if (info.type == NOOMC_GLOBAL) return noomC_emit_AuD(compiler->target, NOOMV_INSTR_SETGLOBAL, compiler->curstack++, (unsigned short)info.idx);
}

void noomC_compiler_init(noomC_Compiler* compiler) {
	compiler->parent = 0;
	compiler->target = 0;

	compiler->localc = 0;
	compiler->upvalc = 0;
	compiler->curstack = 0;
}

noom_Exit noomC_emit(noomV_Function* func, noomV_Inst inst) {
	noomV_Inst* newCode = noom_realloc(func->code, sizeof(noomV_Inst) * (func->codesize + 1));
	if (newCode == 0) return NOOM_ENOMEM;
	func->code = newCode;
	func->code[func->codesize++] = inst;
	return NOOM_OK;
}

noom_Exit noomC_addconst(noomV_Function* func, noomV_Value val) {
	if (func->constsize == NOOM_USHORT_MAX) return NOOM_PLEASEHELPMEIAMSCARED;
	noomV_Value* newConsts = noom_realloc(func->consts, sizeof(noomV_Value) * (func->constsize + 1));
	if (newConsts == 0) return NOOM_ENOMEM;
	func->consts = newConsts;
	func->consts[func->constsize++] = val;
	return NOOM_OK;
}

noomV_String* noomC_internString(const noomC_Compiler* c, noom_LuaVM* vm, const char* str, noom_uint_t len) {
	while (c) {
		const noomV_Function* f = c->target;
		for (int i = 0; i < f->constsize; i++) {
			const noomV_Value v = f->consts[i];
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

noom_Exit noomC_addconst_str(noomC_Compiler* c, noom_LuaVM* vm, const char* str, noom_uint_t len) {
	noomV_String* s = noomC_internString(c, vm, str, len);
	if (s == 0) return NOOM_ENOMEM;
	return noomC_addconst(c->target, (noomV_Value){.tag = NOOMV_VOBJ, .autoclose = 0, .isptr = 0, .obj = (noomV_Object*)s});
}

noom_BinOp noomC_lex_bin_op(const noomP_Parser* parser, noom_uint_t offset) {
	const char* op = parser->code + offset;
	// I AM THE LEXER NOW!!!!!!!!!
	if (noom_startswith(op, "==")) return NOOM_BIN_EQL;
	if (noom_startswith(op, "~=")) return NOOM_BIN_NEQL;
	if (noom_startswith(op, "<=")) return NOOM_BIN_LESSEQL;
	if (noom_startswith(op, ">=")) return NOOM_BIN_GREATEREQL;
	if (noom_startswith(op, "<")) return NOOM_BIN_LESS;
	if (noom_startswith(op, ">")) return NOOM_BIN_GREATER;
	// no .., that is special cased due to funky behavior

	if (parser->version >= NOOM_VERSION_53) {
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

noom_UnaryOp noomC_lex_un_op(const noomP_Parser* parser, noom_uint_t offset) {
	const char* op = parser->code + offset;
	// this can probably be simplified as just first character checks to avoid calling functions lol
	if (noom_startswith(op, "-")) return NOOM_UNARY_NEGATE;
	if (noom_startswith(op, "~")) return NOOM_UNARY_BNOT;
	if (noom_startswith(op, "#")) return NOOM_UNARY_LEN;
	if (noom_startswith(op, "not")) return NOOM_UNARY_NOT;
	return 0;
}

static const char* noomC_decode_string_token(const char* s, noom_LuaVersion how_the_fuck, noom_uint_t* outlen) {
	// nah fuck it TODO atom pls save me
	// actually don't i will repair this one day
	return s;
}

noom_Exit noomC_compile_proto(
    noom_LuaVM* vm,
    noomC_Compiler* parent_compiler,
    const noomP_Parser* parser,
    noomV_Function* parent_func,
    const noomP_Node* params_node,
    const noomP_Node* block_node,
    noom_bool_t has_self,
    noomV_Function** out_proto) {
	noom_Exit result;

	noomV_Function* proto = noomV_allocFunc(vm, parent_func->chunkname);
	if (proto == 0) return NOOM_ENOMEM;

	noomC_Compiler child;
	noomC_compiler_init(&child);
	child.parent = parent_compiler;
	child.target = proto;

	// [varname..., vararg?]
	noom_uint_t param_count = 0;
	noom_bool_t has_vararg = 0; // do we even need that
	for (noom_uint_t i = 0; i < params_node->subnodec; i++) {
		if (params_node->subnodes[i]->type == NOOMP_NODE_VARARG) {
			has_vararg = 1;
		} else {
			param_count++;
		}
	}

	if (has_self) {
		noomC_Local self_local;
		self_local.startpc = 0;
		self_local.endpc = 0;
		self_local.name = "self";
		self_local.namelen = 4;
		self_local.stackslot = 0;
		self_local.dropped = 0;
		self_local.constant = 0;
		self_local.close = 0;
		if ((result = noomC_addLocal(&child, self_local)) != NOOM_OK) return result;
		child.curstack = 1;
		param_count++;
	} else {
		child.curstack = 0;
	}

	proto->argc = (unsigned char)param_count;

	// register arguments as locals
	for (noom_uint_t i = 0; i < params_node->subnodec; i++) {
		const noomP_Node* p = params_node->subnodes[i];
		if (p->type == NOOMP_NODE_VARARG) continue;
		const noom_uint_t length = noomL_tokenlen(parser->code, p->source_offset, parser->version);
		noomC_Local a;
		a.startpc = 0;
		a.endpc = 0;
		a.dropped = 0;
		a.close = 0;
		a.constant = 0;
		a.name = parser->code + p->source_offset;
		a.namelen = length;
		a.stackslot = child.curstack++;
		if ((result = noomC_addLocal(&child, a)) != NOOM_OK) return result;
	}

	if ((result = noomC_compile_block(vm, &child, parser, proto, block_node)) != NOOM_OK) return result;

	noomV_Function** newProtos = noom_realloc(parent_func->protos, sizeof(noomV_Function*) * (parent_func->protosize + 1));
	if (newProtos == 0) return NOOM_ENOMEM;
	parent_func->protos = newProtos;
	parent_func->protos[parent_func->protosize++] = proto;

	*out_proto = proto;
	return NOOM_OK;
}

noom_Exit noomC_emit_assign_to(
    noom_LuaVM* vm,
    noomC_Compiler* compiler,
    const noomP_Parser* parser,
    noomV_Function* func,
    const noomP_Node* target,
    unsigned char value_slot) {
	noom_Exit result;

	if (target->type == NOOMP_NODE_VARIABLE) {
		const char* varname = parser->code + target->source_offset;
		noom_uint_t namelen = noomL_tokenlen(varname, 0, parser->version);
		return noomC_identifyLocalAndSet(compiler, varname, namelen);
	}

	if (target->type == NOOMP_NODE_GETFIELD) {
		// TODO
		return NOOM_OK;
	}

	if (target->type == NOOMP_NODE_INDEX) {
		// TODO
		return NOOM_OK;
	}

	return NOOM_EINTERNAL;
}

// mmap ahh function 🥀🥀🥀🥀🥀🥀🥀
// What 6 argument syscalls there even are other than mmap and clone and is there anything longer
noom_Exit noomC_compile_expr(
    noom_LuaVM* vm,
    noomC_Compiler* compiler,
    const noomP_Parser* parser,
    noomV_Function* func,
    const noomP_Node* node,
    // retc of -1 means all values!!!!!!!!!!!
    int retc) {
	noom_Exit result;

	if (node->type == NOOMP_NODE_NILLITERAL) {
		compiler->curstack++;
		if (func->codesize > 0 && func->code[func->codesize - 1].op == NOOMV_INSTR_PUSHNIL) {
			func->code[func->codesize - 1].us++;
			return NOOM_OK;
		}
		return noomC_emit_AuD(func, NOOMV_INSTR_PUSHNIL, 0, 1);
	}

	if (node->type == NOOMP_NODE_BOOLEANLITERAL) {
		compiler->curstack++;
		noom_uint_t length = noomL_tokenlen(parser->code, node->source_offset, parser->version);
		noom_bool_t val = noom_memeq(parser->code + node->source_offset, length, "true", 4);
		// TODO: analyze last instruction to optimize automatically	
		return noomC_emit_AuD(func, NOOMV_INSTR_PUSHBOOLS, 0, val ? 1 : 0);
	}

	if (node->type == NOOMP_NODE_NUMBERLITERAL) {
		const unsigned short const_idx = func->constsize;
		noomV_Value val = noom_tonumber_except_different_name_so_public_fucking_api_works(parser->code + node->source_offset, 0, parser->version);
		// TODO!!!!11 intern val
		compiler->curstack++;
		return noomC_emit_AuD(func, NOOMV_INSTR_PUSHCONST, 0, const_idx);
	}

	if (node->type == NOOMP_NODE_STRINGLITERAL) {
		const unsigned short const_idx = func->constsize;
		noom_uint_t len;
		const char* sptr = noomC_decode_string_token(parser->code + node->source_offset, parser->version, &len);
		if ((result = noomC_addconst_str(compiler, vm, sptr, len)) != NOOM_OK) return result;
		compiler->curstack++;
		return noomC_emit_AuD(func, NOOMV_INSTR_PUSHCONST, 0, const_idx);
	}

	if (node->type == NOOMP_NODE_VARARGLITERAL) {
		// TODO how do i even push all (retc) varargs
		return NOOM_EINTERNAL;
	}

	if (node->type == NOOMP_NODE_PARENTHESIZED) {
		return noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1);
	}

	if (node->type == NOOMP_NODE_TABLELITERAL) {
		if ((result = noomC_emit_AuD(func, NOOMV_INSTR_CREATETABLE, 0, node->subnodec))) return result;
		const unsigned table_slot = compiler->curstack++; // FIXME is this right?

		noom_uint_t array_idx = 1; // 1 BASED LUA ARRAY INDEXATION!!!!!!!!! DAMN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		for (noom_uint_t i = 0; i < node->subnodec; i++) {
			const noomP_Node* entry = node->subnodes[i];

			if (entry->type == NOOMP_NODE_TABLEENTRY) {
				// [expr, expr] wow how profound and insightful
				const noomP_Node* key_node = entry->subnodes[0];
				const noomP_Node* val_node = entry->subnodes[1];

				if (key_node->type == NOOMP_NODE_FIELDNAME) {
					// string key
					const unsigned short constidx = func->constsize;
					const noom_uint_t length = noomL_tokenlen(parser->code, key_node->source_offset, parser->version);
					if ((result = noomC_addconst_str(compiler, vm, parser->code + key_node->source_offset, length))) return result;
					if ((result = noomC_compile_expr(vm, compiler, parser, func, val_node, 1))) return result;
					compiler->curstack--;
					if ((result = noomC_emit_AuD(func, NOOMV_INSTR_SETFIELD, table_slot, constidx))) return result;
				} else {
					if ((result = noomC_compile_expr(vm, compiler, parser, func, key_node, 1))) return result;
					if ((result = noomC_compile_expr(vm, compiler, parser, func, val_node, 1))) return result;
					compiler->curstack -= 2;
					if ((result = noomC_emit_ABC(func, NOOMV_INSTR_SETTABLE, table_slot, 0, 0))) return result;
				}
			} else {
				const noom_bool_t is_last = i == node->subnodec - 1;
				if ((result = noomC_compile_expr(vm, compiler, parser, func, entry, is_last ? -1 : 1))) return result;
				if (is_last) {
					if ((result = noomC_emit_AuD(func, NOOMV_INSTR_SETLIST, table_slot, (unsigned short)array_idx))) return result;
					compiler->curstack = table_slot + 1;
				} else {
					array_idx++;
				}
			}
		}

		return NOOM_OK;
	}

	if (node->type == NOOMP_NODE_UNARYOPERATOR) {
		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1))) return result;
		return noomC_emit_ABC(func, NOOMV_INSTR_OP, 0, noomC_lex_un_op(parser, node->source_offset), 0);
	}

	if (node->type == NOOMP_NODE_BINARYOPERATOR) {
		if (node->subnodec != 2) return NOOM_EINTERNAL;
		const char* op = parser->code + node->source_offset;
		if (noom_startswith(op, "..")) {
			// concat is special
			noom_uint_t amount = 1;
			const noomP_Node* n = node->subnodes[1];
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1))) return result;
			// we have no flattening so we unwind it ourselves
			// unlike all left-associative ones, like +, where 1 + 2 + 3 is (1 + 2) + 3,
			// concat is right-associate, like ^, meaning 1 .. 2 .. 3 is 1 .. (2 .. 3).
			// VM will handle the concat order within the array for us
			while (1) {
				if (amount > NOOM_USHORT_MAX) return NOOM_EINTERNAL;
				amount++;
				if (noom_startswith(parser->code + n->source_offset, "..")) {
					if ((result = noomC_compile_expr(vm, compiler, parser, func, n->subnodes[0], 1))) return result;
					n = n->subnodes[1];
				} else {
					if ((result = noomC_compile_expr(vm, compiler, parser, func, n, 1))) return result;
					break;
				}
			}
			compiler->curstack -= (unsigned)amount;
			compiler->curstack++;
			return noomC_emit_AuD(func, NOOMV_INSTR_CONCAT, 0, (unsigned short)(amount - 1));
		}

		if (noom_startswith(op, "and")) {
			// TODO *yawn*
			return NOOM_EINTERNAL;
		}

		if (noom_startswith(op, "or")) {
			// TODO *yawn*
			return NOOM_EINTERNAL;
		}

		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1))) return result;
		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[1], 1))) return result;
		// this consumes 2 operands and pushes 1 value, thus having a net stack effect of removing 1 item
		compiler->curstack--;
		return noomC_emit_ABC(func, NOOMV_INSTR_OP, 1, noomC_lex_bin_op(parser, node->source_offset), 0);
	}

	if (node->type == NOOMP_NODE_GETFIELD) {
		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1))) return result;
		noom_uint_t constidx = func->constsize;
		// dumbass oneliner
		if ((result = noomC_addconst_str(compiler, vm, parser->code + node->subnodes[1]->source_offset, noomL_tokenlen(parser->code, node->subnodes[1]->source_offset, parser->version)))) return result;
		return noomC_emit_AuD(func, NOOMV_INSTR_GETFIELD, 0, (unsigned short)constidx);
	}

	if (node->type == NOOMP_NODE_INDEX) {
		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1))) return result;
		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[1], 1))) return result;
		// FIXME we probably want to move the compiler->curstack here
		return noomC_emit_ABC(func, NOOMV_INSTR_GETTABLE, 0, 0, 0);
	}

	if (node->type == NOOMP_NODE_CALL ||
		node->type == NOOMP_NODE_METHODCALL ||
		node->type == NOOMP_NODE_STRINGCALL ||
		node->type == NOOMP_NODE_TABLECALL ||
		node->type == NOOMP_NODE_STRINGMETHODCALL ||
		node->type == NOOMP_NODE_TABLEMETHODCALL) {
		// welcome to multivalue hell
		const unsigned char returnToGlory = (unsigned char)compiler->curstack;
		const noom_bool_t isMethod = node->type == NOOMP_NODE_METHODCALL || node->type == NOOMP_NODE_STRINGMETHODCALL || node->type == NOOMP_NODE_TABLEMETHODCALL;

		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1))) return result;
		const unsigned char funcIdx = (unsigned char)(compiler->curstack - 1);

		if (isMethod) {
			const char* fieldname = parser->code + node->subnodes[1]->source_offset;
			const noom_uint_t fieldlen = noomL_tokenlen(parser->code, node->subnodes[1]->source_offset, parser->version);
			noom_uint_t constidx = func->constsize;
			if ((result = noomC_addconst_str(compiler, vm, fieldname, fieldlen))) return result;
			compiler->curstack++; // self is pushed alongside the method
			if ((result = noomC_emit_AuD(func, NOOMV_INSTR_GETMETHOD, funcIdx, (unsigned short)constidx))) return result;
		}

		// push arguments
		if (node->type == NOOMP_NODE_STRINGCALL || node->type == NOOMP_NODE_TABLECALL) {
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[1], 1))) return result;
		} else if (node->type == NOOMP_NODE_STRINGMETHODCALL || node->type == NOOMP_NODE_TABLEMETHODCALL) {
			if (node->subnodec > 2) {
				if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[2], 1))) return result;
			}
		} else {
			for (noom_uint_t i = isMethod ? 2 : 1; i < node->subnodec; i++) {
				const noom_bool_t isLast = i == node->subnodec - 1;
				if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[i], isLast ? -1 : 1))) return result;
			}
		}

		if ((result = noomC_emit_AuD(func, NOOMV_INSTR_CALL, funcIdx,( unsigned short)(retc < 0 ? 0 : retc + 1)))) return result;

		if (retc >= 0) {
			compiler->curstack = returnToGlory + (unsigned)retc;
		}
		return NOOM_OK;
	}

	if (node->type == NOOMP_NODE_VARIABLE) {
		const char* varname = parser->code + node->source_offset;
		noom_uint_t namelen = noomL_tokenlen(varname, 0, parser->version);
		return noomC_identifyLocalAndSet(compiler, varname, namelen);
	}

	if (node->type == NOOMP_NODE_LAMBDAFUNCTIONLITERAL) {
		const noomP_Node* params_node = node->subnodes[0];
		const noomP_Node* block_node = node->subnodes[1];

		noomV_Function* proto;
		if ((result = noomC_compile_proto(vm, compiler, parser, func, params_node, block_node, 0, &proto))) return result;

		const unsigned short proto_idx = (unsigned short)(func->protosize - 1);
		compiler->curstack++;
		return noomC_emit_AuD(func, NOOMV_INSTR_PUSHCLOSURE, 0, proto_idx);
	}

	return NOOM_EINTERNAL;
}

noom_Exit noomC_compile_block(noom_LuaVM* vm, noomC_Compiler* compiler, const noomP_Parser* parser, noomV_Function* func, const noomP_Node* node) {
	for (noom_uint_t i = 0; i < node->subnodec; i++) {
		noom_Exit r = noomC_add_stuff_to_function(vm, compiler, parser, func, node->subnodes[i]);
		if (r != NOOM_OK) return r;
	}
	return noomC_emit_AuD(func, NOOMV_INSTR_RET, 0, 1);
}

// They say you use only 10% of your brain
// Keep it
// If you think too much you will get headaches

noom_Exit noomC_add_stuff_to_function(noom_LuaVM* vm, noomC_Compiler* compiler, const noomP_Parser* parser, noomV_Function* func, const noomP_Node* node) {
	noom_Exit result;

	// local name1, name2 <attr> = expr1, expr2, ...
	if (node->type == NOOMP_NODE_LOCALDECLARATION) {
		// [VARNAME..., expr...]
		noom_uint_t name_count = 0;
		while (name_count < node->subnodec && node->subnodes[name_count]->type == NOOMP_NODE_VARNAME) name_count++;
		noom_uint_t value_count = node->subnodec - name_count;

		unsigned int base_slot = compiler->curstack;

		for (noom_uint_t i = 0; i < value_count; i++) {
			noom_bool_t is_last = i == value_count - 1;
			int wanted = is_last ? (int)(name_count - i) : 1;
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[name_count + i], wanted))) return result;
		}

		for (noom_uint_t i = 0; i < name_count; i++) {
			const noomP_Node* varname_node = node->subnodes[i];
			const noom_uint_t length = noomL_tokenlen(parser->code, varname_node->source_offset, parser->version);

			noomC_Local local;
			local.startpc = func->codesize;
			local.endpc = 0;
			local.name = parser->code + varname_node->source_offset;
			local.namelen = length;
			local.stackslot = base_slot + i;
			local.dropped = 0;
			local.constant = 0;
			local.close = 0;

			for (noom_uint_t ai = 1; ai < varname_node->subnodec; ai++) {
				noom_uint_t offset = varname_node->subnodes[ai]->source_offset;
				noom_uint_t alength = noomL_tokenlen(parser->code, offset, parser->version);
				const char* ap = parser->code + offset;
				noom_uint_t al = alength;
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

			if ((result = noomC_addLocal(compiler, local))) return result;
		}

		return NOOM_OK;
	}

	// local function name(...) ... end
	if (node->type == NOOMP_NODE_LOCALFUNCTIONDECLARATION) {
		if (node->subnodec != 3) return NOOM_EINTERNAL;
		// Funny staircase huh
		// please autoformatter be a good boy or a good girl or something don't remove it :(
		// be a good person.
		const noomP_Node* varname_node = node->subnodes[0];
		const noomP_Node* params_node = node->subnodes[1];
		const noomP_Node* block_node = node->subnodes[2];
		if (varname_node->type     != NOOMP_NODE_VARNAME) return NOOM_EINTERNAL;
		if (params_node->type     != NOOMP_NODE_FUNCTIONPARAMETERS) return NOOM_EINTERNAL;
		if (block_node->type     != NOOMP_NODE_BLOCK) return NOOM_EINTERNAL;

		noom_uint_t length = noomL_tokenlen(parser->code, varname_node->source_offset, parser->version);
		unsigned int self_slot = compiler->curstack++;

		noomV_Function* proto;
		if ((result = noomC_compile_proto(vm, compiler, parser, func, params_node, block_node, 0, &proto))) return result;

		unsigned short proto_idx = (unsigned short)(func->protosize - 1);
		if ((result = noomC_emit_AuD(func, NOOMV_INSTR_PUSHCLOSURE, (unsigned char)self_slot, proto_idx))) return result;

		noomC_Local local;
		local.startpc = proto->codesize;
		local.endpc = 0;
		local.name = parser->code + varname_node->source_offset;
		local.namelen = length;
		local.stackslot = self_slot;
		local.dropped = 0;
		local.constant = 0;
		local.close = 0;

		return noomC_addLocal(compiler, local);
	}

	// function shit.mp3.opus:method(...) ... end
	if (node->type == NOOMP_NODE_FUNCTIONDECLARATION) {
		// [FUNCTION_NAME[FIELDNAME..., METHODNAME?], FUNCTIONPARAMETERS[VARNAME...], BLOCK
		if (node->subnodec != 3) return NOOM_EINTERNAL;
		const noomP_Node* fname_node = node->subnodes[0];
		const noomP_Node* params_node = node->subnodes[1];
		const noomP_Node* block_node = node->subnodes[2];

		noom_bool_t has_self = 0;
		if (fname_node->subnodec > 0) {
			const noomP_Node* last = fname_node->subnodes[fname_node->subnodec - 1];
			if (last->type == NOOMP_NODE_METHODNAME) has_self = 1;
		}

		noomV_Function* proto;
		if ((result = noomC_compile_proto(vm, compiler, parser, func, params_node, block_node, has_self, &proto))) return result;

		unsigned short proto_idx = (unsigned short)(func->protosize - 1);
		compiler->curstack++;
		if ((result = noomC_emit_AuD(func, NOOMV_INSTR_PUSHCLOSURE, 0, proto_idx))) return result;

		// now assign it uhhh
		if (fname_node->subnodec == 0) return NOOM_EINTERNAL;

		noom_uint_t chain_len = fname_node->subnodec;

		if (chain_len == 1) {
			// function foo() => assign closure to global foo
			// TODO the fuck i look i know
			return NOOM_EINTERNAL;
		} else {
			// TODO idk either
			return NOOM_EINTERNAL;
		}

		compiler->curstack--;
		return NOOM_OK;
	}

	// return expr1, expr2, ...
	if (node->type == NOOMP_NODE_RETURN) {
		if (node->subnodec == 0) {
			return noomC_emit_AuD(func, NOOMV_INSTR_RET, 0, 1);
		}

		unsigned char base_slot = (unsigned char)compiler->curstack;

		for (noom_uint_t i = 0; i < node->subnodec; i++) {
			noom_bool_t is_last = i == node->subnodec - 1;
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[i], is_last ? -1 : 1))) return result;
		}

		return noomC_emit_AuD(func, NOOMV_INSTR_RET, base_slot, 0);
	}

	// a, b.c, d[e] = expr1, expr2, ...
	if (node->type == NOOMP_NODE_ASSIGNMENT) {
		noom_uint_t place_count = 0;
		while (place_count < node->subnodec && node->subnodes[place_count]->type == NOOMP_NODE_ASSIGNPLACE) place_count++;
		noom_uint_t value_count = node->subnodec - place_count;

		unsigned char value_base = (unsigned char)compiler->curstack;
		for (noom_uint_t i = 0; i < value_count; i++) {
			noom_bool_t is_last = i == value_count - 1;
			int wanted = is_last ? (int)(place_count - i) : 1;
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[place_count + i], wanted))) return result;
		}

		for (int i = (int)place_count - 1; i >= 0; i--) {
			unsigned char val_slot = (unsigned char)(value_base + i);
			const noomP_Node* target = node->subnodes[i]->subnodes[0];
			if ((result = noomC_emit_assign_to(vm, compiler, parser, func, target, val_slot))) return result;
		}

		compiler->curstack = value_base;
		return NOOM_OK;
	}

	// if cond then block [elseif cond then block]... [else block] end
	if (node->type == NOOMP_NODE_IFSTATEMENT) {
		// [cond, block, cond, block, ..., block (else, no condition)]

		noom_uint_t* end_jumps = noom_alloc(sizeof(noom_uint_t) * (node->subnodec / 2 + 1));
		if (end_jumps == 0) return NOOM_ENOMEM;
		noom_uint_t end_jump_count = 0;

		noom_uint_t i = 0;
		while (i < node->subnodec) {
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[i], 1)) != NOOM_OK) {
				noom_free(end_jumps);
				return result;
			}
			compiler->curstack--;

			noom_uint_t cond_jmp = func->codesize;
			if ((result = noomC_emit_AuD(func, NOOMV_INSTR_CNJMP, 0, 0)) != NOOM_OK) {
				noom_free(end_jumps);
				return result;
			}

			if ((result = noomC_compile_block(vm, compiler, parser, func, node->subnodes[i + 1])) != NOOM_OK) {
				noom_free(end_jumps);
				return result;
			}

			// if there's more, jump over the remaining branches.
			if (i + 2 < node->subnodec) {
				end_jumps[end_jump_count++] = func->codesize;
				if ((result = noomC_emit_AuD(func, NOOMV_INSTR_JMP, 0, 0)) != NOOM_OK) {
					noom_free(end_jumps);
					return result;
				}
			}

			func->code[cond_jmp].us = (unsigned short)func->codesize;

			i += 2;
		}

		// else
		if (i < node->subnodec) {
			if ((result = noomC_compile_block(vm, compiler, parser, func, node->subnodes[i])) != NOOM_OK) {
				noom_free(end_jumps);
				return result;
			}
		}

		for (noom_uint_t j = 0; j < end_jump_count; j++) {
			func->code[end_jumps[j]].us = (unsigned short)func->codesize;
		}

		noom_free(end_jumps);
		return NOOM_OK;
	}

	// while cond do block end
	if (node->type == NOOMP_NODE_WHILELOOP) {
		// [condition, block]
		if (node->subnodec != 2) return NOOM_EINTERNAL;

		noom_uint_t loop_start = func->codesize;

		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[0], 1))) return result;
		compiler->curstack--;

		noom_uint_t exit_jmp = func->codesize;
		if ((result = noomC_emit_AuD(func, NOOMV_INSTR_CNJMP, 0, 0))) return result;
		if ((result = noomC_compile_block(vm, compiler, parser, func, node->subnodes[1]))) return result;
		if ((result = noomC_emit_AuD(func, NOOMV_INSTR_JMP, 0, (unsigned short)loop_start))) return result;
		func->code[exit_jmp].us = (unsigned short)func->codesize;

		return NOOM_OK;
	}

	// repeat block until cond
	if (node->type == NOOMP_NODE_REPEAT) {
		// [block, cond]
		if (node->subnodec != 2) return NOOM_EINTERNAL;

		noom_uint_t loop_start = func->codesize;

		if ((result = noomC_compile_block(vm, compiler, parser, func, node->subnodes[0]))) return result;
		if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[1], 1))) return result;
		compiler->curstack--;
		return noomC_emit_AuD(func, NOOMV_INSTR_CNJMP, 0, (unsigned short)loop_start);
	}

	// for i = start, stop [, step] do block end
	if (node->type == NOOMP_NODE_FORLOOP) {
		// [VARNAME, start, stop, step?, BLOCK]
		noom_uint_t value_count = node->subnodec - 2; 

		for (noom_uint_t i = 1; i <= value_count; i++) {
			if ((result = noomC_compile_expr(vm, compiler, parser, func, node->subnodes[i], 1))) return result;
		}
		// TODO i don't want to bother doing this
		return NOOM_OK;
	}

	// for k, v in expr do block end
	if (node->type == NOOMP_NODE_FORLOOPIN) {
		// todo nah bro
		return NOOM_OK;
	}

	// break
	if (node->type == NOOMP_NODE_BREAK) {
		// TODO
		return noomC_emit_AuD(func, NOOMV_INSTR_NOP, 0, 0);
	}

	// goto label
	if (node->type == NOOMP_NODE_GOTO) {
		// TODO
		return noomC_emit_AuD(func, NOOMV_INSTR_NOP, 0, 0);
	}

	// ::label::
	if (node->type == NOOMP_NODE_LABEL) {
		// TODO
		return noomC_emit_AuD(func, NOOMV_INSTR_NOP, 0, 0);
	}

	if (node->type == NOOMP_NODE_DOBLOCK || node->type == NOOMP_NODE_BLOCK) {
		return noomC_compile_block(vm, compiler, parser, func, node);
	}

	if (node->type == NOOMP_NODE_CALL ||
		node->type == NOOMP_NODE_METHODCALL ||
		node->type == NOOMP_NODE_STRINGCALL ||
		node->type == NOOMP_NODE_TABLECALL ||
		node->type == NOOMP_NODE_STRINGMETHODCALL ||
		node->type == NOOMP_NODE_TABLEMETHODCALL) {
		return noomC_compile_expr(vm, compiler, parser, func, node, 0);
	}

	return NOOM_EINTERNAL;
}

noom_Exit noomC_compile(noom_LuaVM* vm, const noomP_Parser* parser, const noomP_Node* node, noomV_String* chunkname, noomV_Table* env, noomV_Value* outFunc) {
	if (node->type != NOOMP_NODE_PROGRAM) return NOOM_EINTERNAL;

	noomV_Function* program = noomV_allocFunc(vm, chunkname);
	if (program == 0) return NOOM_ENOMEM;

	outFunc->tag = NOOMV_VOBJ;
	outFunc->obj = &program->obj;

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
		// TODO: we need to wrap the value as a *closure*, with one upvalue, the environment (_ENV).
	}

	return noomC_compile_block(vm, &compiler, parser, program, node);
}
