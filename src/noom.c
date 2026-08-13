#include "noom.h"

#include "compiler.h"
#include "error.h"
#include "helper.h"
#include "parser.h"
#include "vm.h"

// evil bullshit
#include <unistd.h>

static void pretty(const char* code, noom_LuaVersion version, const noomP_Node* node, noom_uint_t indent) {
	for (noom_uint_t i = 0; i < indent; i++) write(STDOUT_FILENO, "\t", 1);
	const noom_uint_t len = noomL_tokenlen(code, node->source_offset, version);
	write(STDOUT_FILENO, code + node->source_offset, len);
	if (code[node->source_offset]) write(STDOUT_FILENO, " -", 2);
	write(STDOUT_FILENO, " ", 1);
	const char *whatever_it_means = noomP_formatNodeType(node->type);
	write(STDOUT_FILENO, whatever_it_means, noom_strlen(whatever_it_means));
	if (node->subnodec) {
		write(STDOUT_FILENO, " with ", 6);
		char wompmwomp = node->subnodec + '0'; // will work for now
		write(STDOUT_FILENO, &wompmwomp, 1);
		write(STDOUT_FILENO, " entr", 5);
		if (node->subnodec == 1) write(STDOUT_FILENO, "y", 1); else write(STDOUT_FILENO, "ies", 3);
		write(STDOUT_FILENO, " {\n", 3);
		for (int i = 0; i < node->subnodec; i++) {
			pretty(code, version, node->subnodes[i], indent + 1);
		}
		for (noom_uint_t i = 0; i < indent; i++) write(STDOUT_FILENO, "\t", 1);
		write(STDOUT_FILENO, "}\n", 2);
	}
	else
		write(STDOUT_FILENO, "\n", 1);
}

// #define LEX_OUTPUT
#define PARSE_OUTPUT
#define COMPILER_OUTPUT

#ifdef LEX_OUTPUT
#include <stdio.h>
#endif

#ifdef COMPILER_OUTPUT
#include <stdio.h>
#endif

noom_Exit
PUBLIC_EXECUTION /* !!!!!!!!!!!!! */
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
(noom_LuaVM* vm, const char* what_am_i, const char* where_am_i, const char *and_most_importantly_who_am_i) {
	noomP_Parser parser;
	noomP_Node* program;

	if (noomP_parse(what_am_i, where_am_i, vm->version, &program, &parser) < 0) {
		const noom_uint_t bleh = noom_format_error(&parser, and_most_importantly_who_am_i, NULL, 0);
		char* buf = noom_alloc(bleh);
		noom_format_error(&parser, and_most_importantly_who_am_i, buf, bleh);
		write(STDERR_FILENO, buf, bleh);
		noomP_freeNode(parser.last_node);
		noom_free(buf);
		return 1;
	}
	
#ifdef LEX_OUTPUT
	puts("LEX OUTPUT:");
	fputs("\x1b[48;2;10;10;10m", stdout);
	noom_uint_t pos = 0;
	while (1) {
		noomL_Token token;

		noomL_ErrorType err = noomL_lex(code, pos, &token, version);
		if (err) break;

		if (token.type == NOOML_TOKEN_KEYWORD) {
			fputs("\x1b[38;2;207;142;109m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		} else if (token.type == NOOML_TOKEN_WHITESPACE) {
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		} else if (token.type == NOOML_TOKEN_IDENTIFIER) {
			fputs("\x1b[38;2;255;255;255m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		} else if (token.type == NOOML_TOKEN_SYMBOL) {
			fputs("\x1b[38;2;0;255;255m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		} else if (token.type == NOOML_TOKEN_STRING) {
			fputs("\x1b[38;2;255;0;0m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		} else if (token.type == NOOML_TOKEN_NUMBER) {
			fputs("\x1b[38;2;0;255;0m", stdout);
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		} else {
			fputs("\x1b[0m\n", stdout);
			printf("%s ", noomL_formatTokenType(token.type));
			for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
			fputs("\x1b[48;2;10;10;10m", stdout);
			putchar('\n');
		}

		pos += token.length;

		if (token.type == NOOML_TOKEN_EOF) break;
	}
	puts("\x1b[0m");
#endif
	
#ifdef PARSE_OUTPUT
	write(STDOUT_FILENO, "PARSE OUTPUT:\n", 14);
	pretty(what_am_i, vm->version, program, 0);
#endif
	noomV_Value peak;

	const noom_Exit e = noomC_compile(vm, &parser, program, NULL, vm->globals, &peak);
	noomP_freeNode(parser.last_node);
	
	if (e) {
		char not_cool[8] = "error: 0";
		not_cool[7] += e; // that will work for now
		write(STDERR_FILENO, not_cool, 8);
		return 1;
	}

#ifdef COMPILER_OUTPUT
	const noomV_Function* f = (noomV_Function*)peak.obj;

	for (int i = 0; i < f->codesize; i++) {
		noomV_Inst inst = f->code[i];
		noomV_DisInfo dis = noomV_disInfo[inst.op];

		printf("%s %d", dis.name, inst.a);

		switch (dis.arg) {
			case NOOMV_DIS_NONE:
				break;
			case NOOMV_DIS_BC:
				printf("%d, %d", inst.b, inst.c);
				break;
			case NOOMV_DIS_uD:
				printf("%d", inst.us);
				break;
			case NOOMV_DIS_sD:
				printf("%d", inst.ss);
				break;
		}
		printf("\n");
	}
#endif
	noomV_pushRawValue(vm, peak);
	noom_Exit err = noom_call(vm, 0, 0);
	if (err != NOOM_OK) {
		char not_cool[9] = "error: 0\n";
		not_cool[7] += err; // that will work for now
		write(STDERR_FILENO, not_cool, 9);
		return err;
	}
	return err;
}
