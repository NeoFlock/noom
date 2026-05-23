#include <stdio.h> // for now
// #include "lexer.h"
#include "parser.h"
#include "helper.h"

void tab(noom_uint_t amount) {
	amount *= 2;
	for (noom_uint_t i = 0; i < amount; i++) {
		putchar(' ');
	}
}

void print_node(noomP_Node* node, noom_uint_t depth) {
	tab(depth);
	printf("{\n");

	tab(depth+1);
	printf("type: %s\n", noomP_formatNodeType(node->type));

	tab(depth+1);
	printf("location: %lld\n", node->source_offset);

	tab(depth+1);
	printf("subnodes:\n");

	for (noom_uint_t i = 0; i < node->subnodec; i++) {
		print_node(node->subnodes[i], depth + 1);
	}

	tab(depth);
	printf("}\n");
}

int main(int argc, char** argv) {
	// uhh uhhh uhhhhh
	const char* code = "local t = {'a'; 2; 6}}";
	noom_uint_t pos = 0;

	printf("LEX OUTPUT:\n");

	noomL_Token token;
	while (1) {
		noomL_lex(code, pos, &token, NOOM_VERSION_54);

		printf("%s ", noomL_formatTokenType(token.type));
		for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		putchar('\n');

		pos += token.length;

		if (token.type == NOOML_TOKEN_EOF) break;
	}

	// time for parser testing
	printf("\nPARSE OUTPUT:\n");

	noomP_Parser parser;
	noomP_Node *program;

	int success = noomP_parse(code, "shitass", NOOM_VERSION_54, &program, &parser);
	if (success == 0)
		print_node(program, 0);

	// freeing time
	noomP_Node *last_node = parser.last_node;
	while (last_node) {
		noomP_Node* next = last_node->previous_node;
		// subnodes could be null if we OOM'd during a realloc of it
		if (last_node->subnodes) noom_free(last_node->subnodes);
		noom_free(last_node);
		last_node = next;
	}
	
	return 0;
}
