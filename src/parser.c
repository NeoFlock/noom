#include "parser.h"
#include "helper.h"

int noomP_peek(noomP_Parser* parser, noomL_Token* token) {
	while (1) {
		int success = noomL_lex(parser->code, parser->lex_offset, token);
		if (success != 0) return -1; // TODO: proper error propogation and stuff

		if (token->type == NOOML_TOKEN_WHITESPACE) {
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

noomP_Node* noomP_parseExpression(noomP_Parser* parser) {
	noomL_Token token;
	noomP_peek(parser, &token);

	if (token.type == NOOML_TOKEN_NUMBER) {
		// uhh figure it out, future me!
		noomP_skip(parser, &token);

		noomP_Node* numNode = noomP_allocNode(parser);
		if (numNode == 0) return 0;
		
		numNode->type = NOOMP_NODE_NUMBERLITERAL;
		numNode->source_offset = token.offset;

		return numNode;
	}

	return 0;
}

noomP_Node* noomP_parseStatement(noomP_Parser* parser) {
	noomL_Token token;
	noomP_peek(parser, &token);

	if (token.type == NOOML_TOKEN_KEYWORD) {
		if (noom_streql(parser->code + token.offset, token.length, "local", 5)) {
			noomP_skip(parser, &token);

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

				noomP_addSubnode(localNode, varname);

				noomP_peek(parser, &token);

				if (token.type == NOOML_TOKEN_SYMBOL) {
					if (noom_streql(parser->code + token.offset, token.length, "=", 1)) {
						noomP_skip(parser, &token);
						break;
					} else if (noom_streql(parser->code + token.offset, token.length, ",", 1)) {
						noomP_skip(parser, &token);
					} else {
						return 0; // unexpected token
					}
				} else {
					return 0; // unexpected token
				}
			}

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
		}
	}

	return 0;
}

int noomP_parse(const char* code, const char* filename, noomP_Node** outpointer, noomP_Node** last_node) {
	noomP_Parser parser;
	noomP_initParser(&parser, code, filename);

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

int noomP_initParser(noomP_Parser* parser, const char* code, const char* filename) {
	parser->code = code;
	parser->filename = filename;
	parser->lex_offset = 0;
	parser->last_node = (void *)0;

	return 0;
}
