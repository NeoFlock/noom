#include "parser.h"
#include "helper.h"

const char *noomP_formatNodeType(noomP_NodeType node_type) {
	switch (node_type) {
		case NOOMP_NODE_PROGRAM:
			return "program";
		case NOOMP_NODE_VARNAME:
			return "varname";
		case NOOMP_NODE_LOCALDECLARATION:
			return "local declaration";
		case NOOMP_NODE_IFSTATEMENT:
			return "if statement";
		case NOOMP_NODE_WHILELOOP:
			return "while loop";
		case NOOMP_NODE_BLOCK:
			return "block";
		case NOOMP_NODE_ATTRIBUTE:
			return "attribute";
		case NOOMP_NODE_BREAK:
			return "break";
		case NOOMP_NODE_VARIABLE:
			return "variable";
		case NOOMP_NODE_NUMBERLITERAL:
			return "number literal";
		case NOOMP_NODE_BOOLEANLITERAL:
			return "boolean literal";
		case NOOMP_NODE_NILLITERAL:
			return "nil literal";
		case NOOMP_NODE_STRINGLITERAL:
			return "string literal";
		case NOOMP_NODE_UNARYOPERATOR:
			return "unary operator";
		case NOOMP_NODE_BINARYOPERATOR:
			return "binary operator";
		case NOOMP_NODE_GETFIELD:
			return "get field";
		case NOOMP_NODE_INDEX:
			return "index";
		case NOOMP_NODE_CALL:
			return "call";
		case NOOMP_NODE_METHODCALL:
			return "method call";
		case NOOMP_NODE_FUNCTIONDECLARATION:
			return "function declaration";
		case NOOMP_NODE_LOCALFUNCTIONDECLARATION:
			return "local function declaration";
		case NOOMP_NODE_FUNCTIONPARAMETERS:
			return "function parameters";
		case NOOMP_NODE_FUNCTIONNAME:
			return "function name";
		case NOOMP_NODE_FIELDNAME:
			return "field name";
		case NOOMP_NODE_METHODNAME:
			return "method name";
		case NOOMP_NODE_VARARG:
			return "vararg";
		case NOOMP_NODE_PARENTHESIZED:
			return "parenthesized";
		case NOOMP_NODE_ASSIGNMENT:
			return "assignment";
		case NOOMP_NODE_ASSIGNPLACE:
			return "assignment place";
		default:
			return "unknown";
	}
}

int noomP_peek(noomP_Parser* parser, noomL_Token* token) {
	while (1) {
		int success = noomL_lex(parser->code, parser->lex_offset, token, parser->version);
		if (success != 0) return -1; // TODO: proper error propogation and stuff

		if (token->type == NOOML_TOKEN_WHITESPACE || token->type == NOOML_TOKEN_COMMENT) {
			// peek changes state, but only if it's one of these useless tokens anyway.
			parser->lex_offset += token->length;
			continue;
		}
		
		return 0;
	}
}

void noomP_skip(noomP_Parser* parser, noomL_Token* token) { // expects you to alr know the token
	if (token->type == NOOML_TOKEN_EOF) return;

	parser->lex_offset += token->length;
}

noomP_Node* noomP_allocNode(noomP_Parser* parser) {
	noomP_Node* node = noom_alloc(sizeof(noomP_Node));
	if (node == 0) return 0;

	node->previous_node = parser->last_node;

	node->subnodec = 0;
	node->subnodes = noom_alloc(sizeof(noomP_Node*) * 2);
	node->subnode_cap = 2;
	if (node->subnodes == 0) {
		noom_free(node);
		return 0;
	}

	parser->last_node = node;

	return node;
}

int noomP_addSubnode(noomP_Node* node, noomP_Node* subnode) {
	if (node->subnodec == node->subnode_cap) {
		node->subnode_cap = node->subnode_cap * 2;
		node->subnodes = noom_realloc(node->subnodes, sizeof(noomP_Node*) * node->subnode_cap);

		if (node->subnodes == 0) return -1;
	}

	node->subnodes[node->subnodec++] = subnode;

	return 0;
}

noomP_Node* noomP_parseComplexExpression(noomP_Parser* parser, noomP_Node* snode) {
	noomP_Node* node = snode;
	noomL_Token token;
	
	while (1) {
		noomP_peek(parser, &token);

		if (token.type == NOOML_TOKEN_SYMBOL) {
			if (noom_streql(parser->code + token.offset, token.length, ".", 1)) { // field
				noomP_skip(parser, &token); // skip the .
				noom_uint_t dotLoc = token.offset;

				noomP_peek(parser, &token);
				if (token.type != NOOML_TOKEN_IDENTIFIER) return 0;
				noomP_skip(parser, &token); // skip the field name

				noomP_Node* new = noomP_allocNode(parser);
				if (new == 0) return 0;

				new->source_offset = dotLoc;
				new->type = NOOMP_NODE_GETFIELD;

				noomP_Node* fname = noomP_allocNode(parser);
				if (new == 0) return 0;

				fname->source_offset = token.offset;
				fname->type = NOOMP_NODE_FIELDNAME;

				noomP_addSubnode(new, node);
				noomP_addSubnode(new, fname);
				
				node = new;
			} else if (noom_streql(parser->code + token.offset, token.length, "[", 1)) { // index
				noomP_skip(parser, &token); // skip the [
				noom_uint_t brackLoc = token.offset;

				noomP_Node* expr = noomP_parseExpression(parser);
				if (expr == 0) return 0;

				noomP_peek(parser, &token); // look for ]
				if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, "]", 1))) {
					// damn it :(
					return 0;
				}
				noomP_skip(parser, &token); // skip ]

				noomP_Node* new = noomP_allocNode(parser);
				if (new == 0) return 0;

				new->type = NOOMP_NODE_INDEX;
				new->source_offset = brackLoc;

				noomP_addSubnode(new, node);
				noomP_addSubnode(new, expr);

				node = new;
			} else if (noom_streql(parser->code + token.offset, token.length, "(", 1)) {
				noomP_skip(parser, &token); // bye (
				noom_uint_t parenLoc = token.offset;

				noomP_Node* new = noomP_allocNode(parser);
				if (new == 0) return 0;

				noomP_addSubnode(new, node);

				new->source_offset = parenLoc;
				new->type = NOOMP_NODE_CALL;

				noomP_peek(parser, &token);

				if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, ")", 1))) {
					while (1) {
						noomP_Node* expr = noomP_parseExpression(parser);
						if (expr == 0) return 0;

						noomP_addSubnode(new, expr);

						noomP_peek(parser, &token);
						if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, ",", 1))) {
							break;
						}
						noomP_skip(parser, &token);
					}
				}

				// check for )
				noomP_peek(parser, &token);
				if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, ")", 1))) {
					return 0;
				}

				noomP_skip(parser, &token);

				node = new;
			} else if (noom_streql(parser->code + token.offset, token.length, ":", 1)) { // method call
				noomP_skip(parser, &token);
				noom_uint_t sym_loc = token.offset;

				noomP_peek(parser, &token); // get method name
				noom_uint_t method = token.offset;
				noomP_skip(parser, &token);

				noomP_Node* new = noomP_allocNode(parser);
				if (new == 0) return 0;

				new->type = NOOMP_NODE_METHODCALL;
				new->source_offset = sym_loc;

				noomP_Node* method_node = noomP_allocNode(parser);
				if (method_node == 0) return 0;

				method_node->type = NOOMP_NODE_FIELDNAME;
				method_node->source_offset = method;

				noomP_addSubnode(new, node);
				noomP_addSubnode(new, method_node);

				noomP_peek(parser, &token);
				if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, "(", 1))) {
					return 0;
				}
				noomP_skip(parser, &token);

				noomP_peek(parser, &token);
				if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, ")", 1))) {
					while (1) {
						noomP_Node* expr = noomP_parseExpression(parser);
						if (expr == 0) return 0;

						noomP_addSubnode(new, expr);

						noomP_peek(parser, &token);
						if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, ",", 1))) {
							break;
						}
						noomP_skip(parser, &token);
					}
				}

				// check for )
				noomP_peek(parser, &token);
				if (token.type != NOOML_TOKEN_SYMBOL || (!noom_streql(parser->code + token.offset, token.length, ")", 1))) {
					return 0;
				}

				noomP_skip(parser, &token);

				node = new;
			} else {
				return node; // done
			}
		} else {
			return node; // done
		}
	}
}

noomP_Node* noomP_parseRawExpression(noomP_Parser* parser) {
	noomL_Token token;
	noomP_peek(parser, &token);

	if (token.type == NOOML_TOKEN_NUMBER) {
		noomP_skip(parser, &token);

		noomP_Node* numNode = noomP_allocNode(parser);
		if (numNode == 0) return 0;
		
		numNode->type = NOOMP_NODE_NUMBERLITERAL;
		numNode->source_offset = token.offset;

		return numNode;
	} else if (token.type == NOOML_TOKEN_STRING) {
		noomP_skip(parser, &token);

		noomP_Node* numNode = noomP_allocNode(parser);
		if (numNode == 0) return 0;
		
		numNode->type = NOOMP_NODE_STRINGLITERAL;
		numNode->source_offset = token.offset;

		return numNode;
	} else if (token.type == NOOML_TOKEN_IDENTIFIER) {
		noomP_skip(parser, &token);

		noomP_Node* varNode = noomP_allocNode(parser);
		if (varNode == 0) return 0;

		varNode->type = NOOMP_NODE_VARIABLE;
		varNode->source_offset = token.offset;

		varNode = noomP_parseComplexExpression(parser, varNode); // complexify
		if (varNode == 0) return 0;

		return varNode;
	} else if (token.type == NOOML_TOKEN_KEYWORD) {
		if (noom_streql(parser->code + token.offset, token.length, "true", 4)) {
			noomP_skip(parser, &token);
		
			noomP_Node* litNode = noomP_allocNode(parser);
			if (litNode == 0) return 0;

			litNode->type = NOOMP_NODE_BOOLEANLITERAL;
			litNode->source_offset = token.offset;

			return litNode;
		} else if (noom_streql(parser->code + token.offset, token.length, "false", 5)) {
			noomP_skip(parser, &token);
		
			noomP_Node* litNode = noomP_allocNode(parser);
			if (litNode == 0) return 0;

			litNode->type = NOOMP_NODE_BOOLEANLITERAL;
			litNode->source_offset = token.offset;

			return litNode;
		} else if (noom_streql(parser->code + token.offset, token.length, "nil", 3)) {
			noomP_skip(parser, &token);
		
			noomP_Node* litNode = noomP_allocNode(parser);
			if (litNode == 0) return 0;

			litNode->type = NOOMP_NODE_NILLITERAL;
			litNode->source_offset = token.offset;

			return litNode;
		}
	} else if (token.type == NOOML_TOKEN_SYMBOL) {
		if (noom_streql(parser->code + token.offset, token.length, "(", 1)) { // parenthesized
			noomP_skip(parser, &token);
			noom_uint_t sym_loc = token.offset;

			noomP_Node* expr = noomP_parseExpression(parser);
			if (expr == 0) return 0;

			// now to close the parentheses

			noomP_peek(parser, &token);
			if (token.type != NOOML_TOKEN_SYMBOL) return 0; // unexpected
			if (!noom_streql(parser->code + token.offset, token.length, ")", 1)) return 0; // unexpected
			noomP_skip(parser, &token);

			noomP_Node* paren = noomP_allocNode(parser); // it has to be a node cause it limits to one value
			if (paren == 0) return 0;

			paren->type = NOOMP_NODE_PARENTHESIZED;
			paren->source_offset = sym_loc;

			noomP_addSubnode(paren, expr);

			// buttt we're not done YET! it could still go :dsg().dsdh():dsh()

			return noomP_parseComplexExpression(parser, paren); // thank you for being a function :heart:
		}
	}

	return 0;
}

int noomP_infixOperatorBP(noomP_Parser* parser, noomL_Token* token, noom_uint_t* a, noom_uint_t* b) { // todo: maybe make this not pointer? we'll see
	if (token->type == NOOML_TOKEN_SYMBOL) {
		if (noom_streql(parser->code + token->offset, token->length, "+", 1)) {
			*a = 90;
			*b = 100;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, "-", 1)) {
			*a = 90;
			*b = 100;
			return 1;

		} else if (noom_streql(parser->code + token->offset, token->length, "*", 1)) {
			*a = 110;
			*b = 120;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, "/", 1)) {
			*a = 110;
			*b = 120;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, "%", 1)) {
			*a = 110;
			*b = 120;
			return 1;

		} else if (noom_streql(parser->code + token->offset, token->length, "^", 1)) {
			*a = 140;
			*b = 130; // right associative
			return 1;

		} else if (noom_streql(parser->code + token->offset, token->length, "..", 2)) {
			*a = 80;
			*b = 70; // right ass.
			return 1;


		// oh boy.
		} else if (noom_streql(parser->code + token->offset, token->length, "<", 1)) {
			*a = 50;
			*b = 60;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, ">", 1)) {
			*a = 50;
			*b = 60;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, "<=", 2)) {
			*a = 50;
			*b = 60;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, ">=", 2)) {
			*a = 50;
			*b = 60;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, "~=", 2)) {
			*a = 50;
			*b = 60;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, "==", 2)) {
			*a = 50;
			*b = 60;
			return 1;
		}
	} else if (token->type == NOOML_TOKEN_KEYWORD) {
		if (noom_streql(parser->code + token->offset, token->length, "and", 3)) {
			*a = 30;
			*b = 40;
			return 1;
		} else if (noom_streql(parser->code + token->offset, token->length, "or", 2)) {
			*a = 10;
			*b = 20;
			return 1;
		}
	}

	return 0;
}

noom_uint_t noomP_prefixOperatorBP(noomP_Parser* parser, noomL_Token* token) { // todo: maybe make this not pointer? we'll see
	if (token->type == NOOML_TOKEN_SYMBOL) {
		if (noom_streql(parser->code + token->offset, token->length, "-", 1)) {
			return 125;
		} else if (noom_streql(parser->code + token->offset, token->length, "#", 1)) {
			return 125;
		} else if (noom_streql(parser->code + token->offset, token->length, "~", 1)) {
			return 125;
		}
	} else if (token->type == NOOML_TOKEN_KEYWORD) {
		if (noom_streql(parser->code + token->offset, token->length, "not", 3)) {
			return 125;
		}
	}

	return 0;
}

noomP_Node* noomP_parseOperatorExpression(noomP_Parser* parser, noom_uint_t min_bp, noomP_Node* predlhs) {
	noomL_Token token;

	noomP_peek(parser, &token);

	noomP_Node* lhs = predlhs;

	// eof check is 2 hard

	if (lhs == 0) { // prefix operator?
		noom_uint_t bp = noomP_prefixOperatorBP(parser, &token);

		if (bp != 0) {
			noomP_skip(parser, &token);

			noomP_Node* child = noomP_parseOperatorExpression(parser, bp, 0);
			if (child == 0) return 0;

			lhs = noomP_allocNode(parser);
			if (lhs == 0) return 0;
			
			lhs->type = NOOMP_NODE_UNARYOPERATOR;
			lhs->source_offset = token.offset; // the operator! we need this to check what it was when compiling.

			noomP_addSubnode(lhs, child);
		}
	}

	// wasn't prefix op, raw?
	if (lhs == 0) {
		noomP_Node* raw = noomP_parseRawExpression(parser);
		if (raw == 0) return 0;

		lhs = raw;
	}

	noom_uint_t lbp;
	noom_uint_t rbp;

	while (1) { // infix operator time!!
		noomP_peek(parser, &token);
		noom_uint_t op_loc = token.offset;

		// also sets lbp and rbp
		int is_op = noomP_infixOperatorBP(parser, &token, &lbp, &rbp);

		if (is_op == 0) break;

		if (lbp < min_bp) break; // joever

		noomP_skip(parser, &token); // remove operator

		noomP_Node* rhs = noomP_parseOperatorExpression(parser, rbp, 0);
		if (rhs == 0) return 0;

		noomP_Node* new_node = noomP_allocNode(parser);
		if (new_node == 0) return 0;

		new_node->type = NOOMP_NODE_BINARYOPERATOR;
		new_node->source_offset = op_loc;

		noomP_addSubnode(new_node, lhs);
		noomP_addSubnode(new_node, rhs);

		lhs = new_node;
	}

	return lhs;
}

noomP_Node* noomP_parseExpression(noomP_Parser* parser) {
	return noomP_parseOperatorExpression(parser, 0, 0);
}

noomP_Node* noomP_parseFunctionParameters(noomP_Parser* parser) {
	noomL_Token token;

	noomP_peek(parser, &token);
	if (token.type != NOOML_TOKEN_SYMBOL || !noom_streql(parser->code + token.offset, token.length, "(", 1)) {
		return 0;
	}
	noomP_skip(parser, &token);

	noomP_Node* params = noomP_allocNode(parser);
	if (params == 0) return 0;

	params->type = NOOMP_NODE_FUNCTIONPARAMETERS;
	params->source_offset = token.offset; // good enough

	// time for the thingies

	while (1) {
		noomP_peek(parser, &token);

		if (token.type == NOOML_TOKEN_IDENTIFIER) {
			noomP_skip(parser, &token);

			noomP_Node* var = noomP_allocNode(parser);
			if (var == 0) return 0;

			var->type = NOOMP_NODE_VARNAME;
			var->source_offset = token.offset;

			noomP_addSubnode(params, var);
		} else if (token.type == NOOML_TOKEN_SYMBOL && noom_streql(parser->code + token.offset, token.length, "...", 3)) {
			noomP_skip(parser, &token);

			noomP_Node* vararg = noomP_allocNode(parser);
			if (vararg == 0) return 0;

			vararg->type = NOOMP_NODE_VARARG;
			vararg->source_offset = token.offset;

			noomP_addSubnode(params, vararg);

			break; // no more allowed.
		}

		noomP_peek(parser, &token);

		if (token.type == NOOML_TOKEN_SYMBOL && noom_streql(parser->code + token.offset, token.length, ",", 1)) {
			noomP_skip(parser, &token);
		} else {
			break;
		}
	}

	// closing paren
	noomP_peek(parser, &token);
	if (token.type != NOOML_TOKEN_SYMBOL || !noom_streql(parser->code + token.offset, token.length, ")", 1)) {
		return 0;
	}
	noomP_skip(parser, &token);

	return params;
}

noomP_Node* noomP_parseBlock(noomP_Parser* parser) { // stops on end, else or elseif.
	// block starter has been eaten already; we just go until ending keyword
	noomP_Node* node = noomP_allocNode(parser);
	if (node == 0) return 0; // OOM :(
	
	node->type = NOOMP_NODE_BLOCK;
	node->source_offset = parser->lex_offset;

	noomL_Token token;
	
	while (1) {
		// check if end reached
		noomP_peek(parser, &token);

		if (token.type == NOOML_TOKEN_KEYWORD) {
			if (noom_streql(parser->code + token.offset, token.length, "end", 3)) {
				break;
			} else if (noom_streql(parser->code + token.offset, token.length, "elseif", 6)) {
				break;
			} else if (noom_streql(parser->code + token.offset, token.length, "else", 4)) {
				break;
			}
		}
	
		noomP_Node* stmt = noomP_parseStatement(parser);
		if (stmt == 0) return 0;

		noomP_addSubnode(node, stmt);
	}

	return node;
}

noomP_Node* noomP_parseRawStatement(noomP_Parser* parser) {
	noomL_Token token;
	noomP_peek(parser, &token);

	if (token.type == NOOML_TOKEN_KEYWORD) {
		if (noom_streql(parser->code + token.offset, token.length, "local", 5)) {
			noomP_skip(parser, &token);

			noomP_peek(parser, &token);

			if (token.type == NOOML_TOKEN_KEYWORD && noom_streql(parser->code + token.offset, token.length, "function", 8)) {
				noomP_skip(parser, &token);
				
				noomP_Node* funcNode = noomP_allocNode(parser);
				if (funcNode == 0) return 0;

				funcNode->type = NOOMP_NODE_LOCALFUNCTIONDECLARATION;
				funcNode->source_offset = token.offset;
				
				noomP_peek(parser, &token);
				if (token.type != NOOML_TOKEN_IDENTIFIER) return 0;
				noomP_skip(parser, &token);
				
				noomP_Node* nameNode = noomP_allocNode(parser);
				if (nameNode == 0) return 0;

				nameNode->type = NOOMP_NODE_VARNAME;
				nameNode->source_offset = token.offset;

				noomP_addSubnode(funcNode, nameNode);

				noomP_Node* params = noomP_parseFunctionParameters(parser);
				if (params == 0) return 0;
				noomP_addSubnode(funcNode, params);

				noomP_Node* block = noomP_parseBlock(parser);
				if (block == 0) return 0;
				noomP_addSubnode(funcNode, block);

				noomP_peek(parser, &token);
				if (token.type != NOOML_TOKEN_KEYWORD || !noom_streql(parser->code + token.offset, token.length, "end", 3)) {
					return 0;
				}
				noomP_skip(parser, &token);

				return funcNode;
			}

			noomP_Node* localNode = noomP_allocNode(parser);
			if (localNode == 0) return 0; // no memory :(

			localNode->source_offset = token.offset;
			localNode->type = NOOMP_NODE_LOCALDECLARATION;

			while (1) {
				noomP_peek(parser, &token);

				if (token.type != NOOML_TOKEN_IDENTIFIER) return 0;
				noomP_skip(parser, &token);

				noomP_Node* varname = noomP_allocNode(parser);
				if (varname == 0) return 0;
				
				varname->type = NOOMP_NODE_VARNAME;
				varname->source_offset = token.offset;

				if (parser->version >= NOOM_VERSION_54) {
					noomP_peek(parser, &token);

					if (token.type == NOOML_TOKEN_SYMBOL && noom_streql(parser->code + token.offset, token.length, "<", 1)) {
						// attribute yay
						noomP_skip(parser, &token);

						// the attribute is an identifier.
						noomP_peek(parser, &token);
						if (token.type != NOOML_TOKEN_IDENTIFIER) return 0; // unexpected
						noom_uint_t attr = token.offset;
						noomP_skip(parser, &token);

						if (!noom_streql(parser->code + token.offset, token.length, "const", 5) && !noom_streql(parser->code + token.offset, token.length, "close", 5)) {
							return 0; // not a real attribute smh my head
						}

						noomP_peek(parser, &token);
						if (token.type != NOOML_TOKEN_SYMBOL) return 0;
						if (!noom_streql(parser->code + token.offset, token.length, ">", 1)) return 0;
						noomP_skip(parser, &token);

						noomP_Node* attrn = noomP_allocNode(parser);
						if (attr == 0) return 0;

						attrn->type = NOOMP_NODE_ATTRIBUTE;
						attrn->source_offset = attr;

						noomP_addSubnode(varname, attrn);
					}
				}
				
				noomP_addSubnode(localNode, varname);

				noomP_peek(parser, &token);

				if (token.type == NOOML_TOKEN_SYMBOL) {
					if (noom_streql(parser->code + token.offset, token.length, ",", 1)) {
						noomP_skip(parser, &token);
					} else {
						break;
					}
				} else {
					break;
				}
			}

			noomP_peek(parser, &token);

			// local with no equals initializes to nil
			if (token.type != NOOML_TOKEN_SYMBOL) return localNode;
			if (!noom_streql(parser->code + token.offset, token.length, "=", 1)) {
				return localNode;
			}
			noomP_skip(parser, &token);

			// equals has already been eaten by loop (thank you loop)

			while (1) {
				noomP_Node *expr = noomP_parseExpression(parser);
				if (expr == 0) return 0;

				noomP_addSubnode(localNode, expr);

				noomP_peek(parser, &token);

				if (token.type == NOOML_TOKEN_SYMBOL) {
					if (noom_streql(parser->code + token.offset, token.length, ",", 1)) {
						noomP_skip(parser, &token);
					} else {
						break;
					}
				} else {
					break;
				}
			}

			return localNode;
		} else if (noom_streql(parser->code + token.offset, token.length, "if", 2)) {
			noomP_skip(parser, &token);

			noomP_Node* ifStatement = noomP_allocNode(parser);
			if (ifStatement == 0) return 0;

			ifStatement->type = NOOMP_NODE_IFSTATEMENT;
			ifStatement->source_offset = token.offset;

			noomP_Node* condition = noomP_parseExpression(parser);
			if (condition == 0) return 0;

			noomP_addSubnode(ifStatement, condition);

			noomP_peek(parser, &token);

			if (token.type != NOOML_TOKEN_KEYWORD) return 0; // unexpected
			if (!noom_streql(parser->code + token.offset, token.length, "then", 4)) return 0; // unexpected

			noomP_skip(parser, &token);

			noomP_Node* block = noomP_parseBlock(parser);
			if (block == 0) return 0;

			noomP_addSubnode(ifStatement, block);

			while (1) { // else, elseif
				noomP_peek(parser, &token);

				if (token.type != NOOML_TOKEN_KEYWORD) return 0; // unexpected

				if (noom_streql(parser->code + token.offset, token.length, "elseif", 6)) {
					noomP_skip(parser, &token);
					noomP_Node* elseIfCondition = noomP_parseExpression(parser);
					if (elseIfCondition == 0) return 0;

					noomP_addSubnode(ifStatement, elseIfCondition);

					// now we need to check for "then"
					noomP_peek(parser, &token);

					if (token.type != NOOML_TOKEN_KEYWORD) return 0; // unexpected
					if (!noom_streql(parser->code + token.offset, token.length, "then", 4)) return 0; // unexpected
					noomP_skip(parser, &token);

					// now the block
					noomP_Node* elseIfBlock = noomP_parseBlock(parser);
					if (elseIfBlock == 0) return 0;

					noomP_addSubnode(ifStatement, elseIfBlock);

					// could be even more
				} else if (noom_streql(parser->code + token.offset, token.length, "else", 4)) {
					noomP_skip(parser, &token);

					noomP_Node* elseBlock = noomP_parseBlock(parser);
					if (elseBlock == 0) return 0;

					// we know it's an else if it's an odd number. no need to do anything special.
					noomP_addSubnode(ifStatement, elseBlock);
				
					break; // this must be the last one; end is handled after the loop
				} else if (noom_streql(parser->code + token.offset, token.length, "end", 3)) {
					break; // will check for end outside the loop because else and things
				} else {
					// unexpected
					return 0;
				}
			}

			noomP_peek(parser, &token);
			if (token.type != NOOML_TOKEN_KEYWORD) return 0; // unexpected
			if (!noom_streql(parser->code + token.offset, token.length, "end", 3)) return 0; // unexpected
			noomP_skip(parser, &token);

			return ifStatement;
		} else if (noom_streql(parser->code + token.offset, token.length, "while", 5)) {
			noomP_skip(parser, &token); // skip `while`

			noomP_Node* node = noomP_allocNode(parser);
			if (node == 0) return 0;

			node->type = NOOMP_NODE_WHILELOOP;
			node->source_offset = token.offset;

			noomP_Node* cond = noomP_parseExpression(parser);
			if (cond == 0) return 0;

			noomP_addSubnode(node, cond);

			noomP_peek(parser, &token);

			if (token.type != NOOML_TOKEN_KEYWORD) return 0; // unexpected
			if (!noom_streql(parser->code + token.offset, token.length, "do", 2)) return 0; // unexpected
			noomP_skip(parser, &token); // skip `do`

			noomP_Node* block = noomP_parseBlock(parser);
			if (block == 0) return 0;

			noomP_addSubnode(node, block);

			noomP_peek(parser, &token);
			if (token.type != NOOML_TOKEN_KEYWORD) return 0; // unexpected
			if (!noom_streql(parser->code + token.offset, token.length, "end", 3)) return 0; // unexpected
			noomP_skip(parser, &token); // skip `end`

			return node;
		} else if (noom_streql(parser->code + token.offset, token.length, "break", 5)) {
			noomP_skip(parser, &token); // skip `break`

			// uhh yeah that's it i guess? idk. maybe parsers should figure out what loop?
			// sounds like a thing for the compiler to bytecode though
			noomP_Node* node = noomP_allocNode(parser);
			if (node == 0) return 0;
			
			node->type = NOOMP_NODE_BREAK;
			node->source_offset = token.offset;

			return node;
		} else if (noom_streql(parser->code + token.offset, token.length, "function", 8)) {
			noomP_skip(parser, &token);

			noomP_Node* func = noomP_allocNode(parser);
			if (func == 0) return 0;

			func->type = NOOMP_NODE_FUNCTIONDECLARATION;
			func->source_offset = token.offset;

			// function names can only have . and one singular :
			// we'll just do it here.

			noomP_Node* fname = noomP_allocNode(parser);
			if (fname == 0) return 0;

			noomP_peek(parser, &token);
			fname->type = NOOMP_NODE_FUNCTIONNAME;
			fname->source_offset = token.offset;

			if (token.type != NOOML_TOKEN_IDENTIFIER) return 0; // unex.
			noomP_skip(parser, &token);

			noomP_Node* base = noomP_allocNode(parser);
			if (base == 0) return 0;

			base->type = NOOMP_NODE_FIELDNAME; // well ok it's technically a variable name but like come on
			base->source_offset = token.offset;

			noomP_addSubnode(fname, base);
		
			while (1) {
				noomP_peek(parser, &token);

				if (token.type != NOOML_TOKEN_SYMBOL) return 0; // unexp.

				if (noom_streql(parser->code + token.offset, token.length, ".", 1)) {
					// just keep on going at it
					noomP_skip(parser, &token);

					noomP_peek(parser, &token);

					if (token.type != NOOML_TOKEN_IDENTIFIER) return 0;
					noomP_skip(parser, &token);

					noomP_Node* new = noomP_allocNode(parser);
					if (new == 0) return 0;

					new->type = NOOMP_NODE_FIELDNAME;
					new->source_offset = token.offset;

					noomP_addSubnode(fname, new);
				} else if (noom_streql(parser->code + token.offset, token.length, ":", 1)) {
					noomP_skip(parser, &token);

					noomP_peek(parser, &token);
					if (token.type != NOOML_TOKEN_IDENTIFIER) return 0;
					noomP_skip(parser, &token);

					noomP_Node* new = noomP_allocNode(parser);
					if (new == 0) return 0;

					new->type = NOOMP_NODE_METHODNAME;
					new->source_offset = token.offset;

					noomP_addSubnode(fname, new);

					break; // ( checked later.
				} else if (noom_streql(parser->code + token.offset, token.length, "(", 1)) {
					break;
				} else {
					return 0;
				}
			}

			noomP_addSubnode(func, fname);

			noomP_Node* params = noomP_parseFunctionParameters(parser);
			if (params == 0) return 0;
			noomP_addSubnode(func, params);

			noomP_Node* block = noomP_parseBlock(parser);
			if (block == 0) return 0;
			noomP_addSubnode(func, block);

			// remove `end`
			noomP_peek(parser, &token);
			if (token.type != NOOML_TOKEN_KEYWORD || !noom_streql(parser->code + token.offset, token.length, "end", 3)) {
				return 0;
			}
			noomP_skip(parser, &token);

			return func;
		}
	}

	if (token.type == NOOML_TOKEN_IDENTIFIER || (token.type == NOOML_TOKEN_SYMBOL && noom_streql(parser->code + token.offset, token.length, "(", 1))) {
		noomP_Node* base = noomP_parseRawExpression(parser);
		if (base == 0) return 0;

		if (base->type == NOOMP_NODE_INDEX || base->type == NOOMP_NODE_GETFIELD || base->type == NOOMP_NODE_VARIABLE) {
			// always assignment i think
			noomP_Node* assignment = noomP_allocNode(parser);
			if (assignment == 0) return 0;

			noomP_peek(parser, &token);
			assignment->type = NOOMP_NODE_ASSIGNMENT;
			assignment->source_offset = token.offset; // probably set this to the `=` later.

			noomP_Node* initial_place = noomP_allocNode(parser);
			if (initial_place == 0) return 0;

			initial_place->type = NOOMP_NODE_ASSIGNPLACE;
			initial_place->source_offset = base->source_offset;
			
			noomP_addSubnode(initial_place, base);

			noomP_addSubnode(assignment, initial_place);

			while (1) {
				noomP_peek(parser, &token);

				if (token.type == NOOML_TOKEN_SYMBOL) {
					if (noom_streql(parser->code + token.offset, token.length, ",", 1)) {
						noomP_skip(parser, &token);
						
						noomP_peek(parser, &token);
						
						if (token.type != NOOML_TOKEN_IDENTIFIER && (token.type != NOOML_TOKEN_SYMBOL || !noom_streql(parser->code + token.offset, token.length, "(", 1))) {
							// unexpected
							return 0;
						}
						
						// more thingers
						noomP_Node* item = noomP_parseRawExpression(parser);
						if (item == 0) return 0;

						if (item->type != NOOMP_NODE_INDEX && item->type != NOOMP_NODE_GETFIELD && item->type != NOOMP_NODE_VARIABLE) {
							return 0; // unexpected
						}

						noomP_Node* container = noomP_allocNode(parser);
						if (container == 0) return 0;

						container->type = NOOMP_NODE_ASSIGNPLACE;
						container->source_offset = item->source_offset;

						noomP_addSubnode(container, item);

						noomP_addSubnode(assignment, container);
					} else if (noom_streql(parser->code + token.offset, token.length, "=", 1)) {
						assignment->source_offset = token.offset;
						noomP_skip(parser, &token);
						break;
					}
				} else {
					return 0; // unexpected
				}
			}

			// `=` has already been eaten at this point.
			// time for the values.

			while (1) {
				noomP_Node* expr = noomP_parseExpression(parser);
				if (expr == 0) return 0;

				noomP_addSubnode(assignment, expr);

				noomP_peek(parser, &token);

				if (token.type == NOOML_TOKEN_SYMBOL && noom_streql(parser->code + token.offset, token.length, ",", 1)) {
					noomP_skip(parser, &token);
					continue; // explicit cause felt like it
				} else {
					break;
				}
			}

			return assignment;
		} else if (base->type == NOOMP_NODE_CALL || base->type == NOOMP_NODE_METHODCALL) {
			// this expression is now a statement.
			return base; // no need to eat any more.
		} else {
			return 0; // unexpected. e.g. random string or whatever
		}
	}


	return 0;
}

noomP_Node* noomP_parseStatement(noomP_Parser* parser) {
	noomL_Token token;
	
	noomP_Node* stmt = noomP_parseRawStatement(parser);
	if (stmt == 0) return 0;

	while (1) {
		noomP_peek(parser, &token);
		if (token.type == NOOML_TOKEN_SYMBOL) {
			if (noom_streql(parser->code + token.offset, token.length, ";", 1)) {
				noomP_skip(parser, &token);
				continue;
			}
		}
		break;
	}

	return stmt;
}

int noomP_parse(const char* code, const char* filename, noom_LuaVersion version, noomP_Node** outpointer, noomP_Node** last_node) {
	noomP_Parser parser;
	noomP_initParser(&parser, code, filename, version);

	noomL_Token token;
	noomP_Node* node = noomP_allocNode(&parser);
	if (node == 0) return -1;

	node->source_offset = parser.lex_offset;
	node->type = NOOMP_NODE_PROGRAM;

	while (1) {
		noomP_peek(&parser, &token);
		if (token.type == NOOML_TOKEN_EOF) break;

		noomP_Node* child = noomP_parseStatement(&parser);
		if (child == 0) return -1;

		noomP_addSubnode(node, child);
	}

	*outpointer = node;
	*last_node = parser.last_node;

	return 0;
}

int noomP_initParser(noomP_Parser* parser, const char* code, const char* filename, noom_LuaVersion version) {
	parser->code = code;
	parser->filename = filename;
	parser->lex_offset = 0;
	parser->last_node = (void *)0;
	parser->version = version;

	return 0;
}
