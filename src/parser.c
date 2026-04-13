#include "parser.h"
#include "helper.h"

int noomP_peek(noomP_Parser* parser, noomL_Token* token) {
	while (1) {
		int success = noomL_lex(parser->code, parser->lex_offset, token);
		if (!success) return -1; // TODO: proper error propogation and stuff
	
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

noomP_Node* noomP_parseStatement(noomP_Parser* parser) {
	return 0;
}

int noomP_parse(const char* code, const char* filename, noomP_Node** outpointer) {
	noomP_Parser parser;
	noomP_initParser(&parser, code, filename);

	noomL_Token token;
	noomP_Node* node = noomP_allocNode(&parser);

	node->type = NOOMP_NODE_PROGRAM;

	while (1) {
		noomP_peek(&parser, &token);
		if (token.type == NOOML_TOKEN_EOF) break;

		noomP_Node* child = noomP_parseStatement(&parser);
		if (child == 0) return -1;

		noomP_addSubnode(node, child);
	}

	node->type = NOOMP_NODE_PROGRAM;
	node->subnodec = 0;

	return 0;
}

int noomP_initParser(noomP_Parser* parser, const char* code, const char* filename) {
	parser->code = code;
	parser->filename = filename;
	parser->lex_offset = 0;
	parser->last_node = (void *)0;

	return 0;
}
