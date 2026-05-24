#include <stdio.h> // for now
// #include "lexer.h"
#include "helper.h"
#include "error.h"

void tab(noom_uint_t amount) {
	amount *= 2;
	for (noom_uint_t i = 0; i < amount; i++) {
		putchar(' ');
	}
}

void print_node(noomP_Node *node, noom_uint_t depth) {
	tab(depth);
	printf("{\n");

	tab(depth + 1);
	printf("type: %s\n", noomP_formatNodeType(node->type));

	tab(depth + 1);
	printf("location: %lld\n", node->source_offset);

	tab(depth + 1);
	printf("subnodes:\n");

	for (noom_uint_t i = 0; i < node->subnodec; i++) {
		print_node(node->subnodes[i], depth + 1);
	}

	tab(depth);
	printf("}\n");
}

int main(int argc, char **argv) {
	// uhh uhhh uhhhhh
	const char *code = "local t <close> = {'a'; 2; 6 \"\\xgg\"";
	noom_uint_t pos = 0;

	printf("LEX OUTPUT:\n");

	fputs("\x1b[48;2;10;10;10m", stdout);
	while (1) {
		noomL_Token token;
		//noomL_lex(code, pos, &token, NOOM_VERSION_54);
		noomL_ErrorType err = noomL_lex(code, pos, &token, NOOM_VERSION_54);
		if (err) break;

		if (token.type == NOOML_TOKEN_KEYWORD) {
			fputs("\x1b[38;2;207;142;109m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
			fflush(stdout);
		} else if (token.type == NOOML_TOKEN_WHITESPACE) {
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		} else if (token.type == NOOML_TOKEN_IDENTIFIER) {
			fputs("\x1b[38;2;255;255;255m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
			fflush(stdout);
		} else if (token.type == NOOML_TOKEN_SYMBOL) {
			fputs("\x1b[38;2;0;255;255m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
			fflush(stdout);
		} else if (token.type == NOOML_TOKEN_STRING) {
			fputs("\x1b[38;2;255;0;0m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
			fflush(stdout);
		} else if (token.type == NOOML_TOKEN_NUMBER) {
			fputs("\x1b[38;2;0;255;0m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
			fflush(stdout);
		} else {
			fputs("\x1b[0m\n", stdout);
			printf("%s ", noomL_formatTokenType(token.type));
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
			fputs("\x1b[48;2;10;10;10m", stdout);
			putchar('\n');
			fflush(stdout);
		}

		pos += token.length;

		if (token.type == NOOML_TOKEN_EOF) break;
	}
	fputs("\x1b[0m\n", stdout);

	// time for parser testing
	printf("\nPARSE OUTPUT:\n");

	noomP_Parser parser;
	noomP_Node *program;

	int success = noomP_parse(code, "shitass", NOOM_VERSION_54, &program, &parser);
	if (success == 0)
		print_node(program, 0);
	else {
		noom_uint_t bleh = noom_format_error(&parser, NULL, 0);
		char* buf = noom_alloc(bleh);
		noom_format_error(&parser, buf, bleh);
		fputs(buf, stdout);
		noom_free(buf);
	}

	// freeing time
	noomP_Node *last_node = parser.last_node;
	while (last_node) {
		noomP_Node *next = last_node->previous_node;
		// subnodes could be null if we OOM'd during a realloc of it
		if (last_node->subnodes) noom_free(last_node->subnodes);
		noom_free(last_node);
		last_node = next;
	}

	return 0;
}
