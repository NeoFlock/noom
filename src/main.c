#include <stdio.h> // for now
#include "helper.h"
#include "error.h"
#include "noom.h"
#include "compiler.h"
#include "vm.h"

volatile const char wawa[] = "if you are reading this with either the `strings` utility or through a hex viewer please know that you can instead just check out the source code on gitea: https://gitea.codersquack.nl/NeoFlock/noom";

void tab(noom_uint_t amount) {
	amount *= 2;
	for (noom_uint_t i = 0; i < amount; i++) {
		putchar(' ');
		putchar(' ');
		putchar(' ');
		putchar(' ');
	}
}

void print_node(const noomP_Node* node, noom_uint_t depth) {
	tab(depth);
	printf("{\n");

	tab(depth + 1);
	printf("type: %s\n", noomP_formatNodeType(node->type));

	tab(depth + 1);
	printf("location: %lld\n", node->source_offset);

	if (node->subnodec > 0) {
		tab(depth + 1);
		printf("subnodes (%llu):\n", node->subnodec);

		for (noom_uint_t i = 0; i < node->subnodec; i++) {
			print_node(node->subnodes[i], depth + 1);
		}
	}

	tab(depth);
	printf("}\n");
}

void pretty(const char* code, noom_LuaVersion version, const noomP_Node* node, noom_uint_t indent) {
	for (noom_uint_t i = 0; i < indent; i++) putchar('\t');
	const noom_uint_t len = noomL_tokenlen(code, node->source_offset, version);
	char* fuckoff = code;
	const char c = fuckoff[node->source_offset + len];
	fuckoff[node->source_offset + len] = '\0';
	printf("%*s%s %s", (int)len, code + node->source_offset, code[node->source_offset] != '\0' ? " -" : "", noomP_formatNodeType(node->type));
	fuckoff[node->source_offset + len] = c;
	if (node->subnodec) {
		printf(" with %lld entr%s {\n", node->subnodec, node->subnodec == 1 ? "y" : "ies");
		for (int i = 0; i < node->subnodec; i++) {
			pretty(code, version, node->subnodes[i], indent + 1);
		}
		for (noom_uint_t i = 0; i < indent; i++) putchar('\t');
		putchar('}');
		putchar('\n');
	}
	else
		putchar('\n');
}

// #define LEX_OUTPUT
#define PARSE_OUTPUT
#define COMPILER_OUTPUT

int execute(const char* code, noom_LuaVersion version, const char* program_name, const char* filename) {
	noomP_Parser parser;
	noomP_Node* program;

	if (noomP_parse(code, filename, version, &program, &parser) < 0) {
		const noom_uint_t bleh = noom_format_error(&parser, program_name, NULL, 0);
		char* buf = noom_alloc(bleh);
		noom_format_error(&parser, program_name, buf, bleh);
		fputs(buf, stdout);
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
	puts("PARSE OUTPUT:");
	pretty(code, version, program, 0);
	//print_node(program, 0);
#endif

	noom_LuaVM* vm = noom_createVM(version);
	noomV_Value peak;

	const noom_Exit e = noomC_compile(vm, &parser, program, 0, 0, &peak);
	if (e) {
		printf("error: %d\n", e);
		noomP_freeNode(parser.last_node);
		return 1;
	}
	noomP_freeNode(parser.last_node);

#ifdef COMPILER_OUTPUT
	noomV_Function* f = (noomV_Function*)peak.obj;

	for (int i = 0; i < f->codesize; i++) {
		noomV_Inst inst = f->code[i];
		noomV_DisInfo dis = noomV_disInfo[inst.op];

		printf("%s %d ", dis.name, inst.a);

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
	noom_destroyVM(vm);
	
	return 0;
}

static char* read_file(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == 0) {
		fprintf(stderr, "Failed to open '%s'.\n", filename);
		return 0;
	}

	fseek(file, 0, SEEK_END);
	const unsigned long filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = noom_alloc(filesize + 1);
	if (buffer == 0) return 0;

	if (fread(buffer, 1, filesize, file) != filesize) {
		fprintf(stderr, "Reached the end of the file\n");
		noom_free(buffer);
		return 0;
	}
	buffer[filesize] = '\0';

	fclose(file);
	return buffer;
}

static char* read_stdin() {
	noom_uint_t capacity = 4096;
	noom_uint_t size = 0;
	char* buffer = noom_alloc(capacity);
	if (buffer == 0) return 0;

	size_t n;
	while ((n = fread(buffer + size, 1, capacity - size, stdin)) > 0) {
		size += n;
		if (size == capacity) {
			capacity *= 2;
			buffer = noom_realloc_free(buffer, capacity);
			if (buffer == 0) return 0;
		}
	}

	buffer[size] = '\0';
	return buffer;
}

static int read_prompt(char* buf, int buf_size, char* prompt, const int required) {
	do {
		printf("%s", prompt);
		fflush(stdout);
		if (!fgets(buf, buf_size, stdin)) return 1;
		const size_t len = noom_strlen(buf);
		if (len > 0 && buf[len - 1] != '\n') {
			while (getchar() != '\n' && !feof(stdin));
		} else if (len > 0) {
			buf[len - 1] = '\0';
		}
	} while (buf[0] == '\0' && required);
	return 0;
}

int main(int argc, char** argv) {
	const char* err = 0;
	struct {
		noom_bool_t enter_repl;
		noom_bool_t use_stdin;
		const char* script_exec;
		const char* script_path;
		noom_bool_t do_i_already_know_what_to_do;
		noom_LuaVersion lua_version;
	} params = {0};

	if (argc < 2) {
		params.enter_repl = 1;
		params.do_i_already_know_what_to_do = 1;
	}

	for (int i = 1; i < argc; i++) {
		if (noom_strcmp(argv[i], "-") == 0) {
			params.use_stdin = 1;
			params.do_i_already_know_what_to_do = 1;
			continue;
		}

		if (noom_strcmp(argv[i], "--") == 0) {
			if (++i >= argc) break;
			if (params.do_i_already_know_what_to_do) {
				err = "too many arguments";
				goto die;
			}
			params.script_exec = argv[i];
			params.do_i_already_know_what_to_do = 1;
			continue;
		}

		if (argv[i][0] != '-') {
			if (params.do_i_already_know_what_to_do) {
				err = "too many arguments";
				goto die;
			}
			params.script_path = argv[i];
			params.do_i_already_know_what_to_do = 1;
			continue;
		}

		if (argv[i][1] == 'e') {
			if (params.do_i_already_know_what_to_do) {
				goto die;
			}
			// "-estat" or "-e stat"
			if (argv[i][2] != '\0') {
				params.script_exec = argv[i] + 2;
				params.do_i_already_know_what_to_do = 1;
				continue;
			}

			if (++i >= argc) {
				err = "-e needs an argument";
				goto die;
			}
			params.script_exec = argv[i];
			params.do_i_already_know_what_to_do = 1;
			i++;
			continue;
		}

		if (argv[i][1] == 'l') {
			const char* version_string = 0;
			if (argv[i][2] != '\0') {
				version_string = argv[i] + 2;
			} else {
				if (++i >= argc) {
					err = "-l needs an argument";
					goto die;
				}
				version_string = argv[i];
				i++;
			}
			if (noom_strcmp(version_string, "51") == 0) params.lua_version = NOOM_VERSION_51;
			else if (noom_strcmp(version_string, "52") == 0) params.lua_version = NOOM_VERSION_52;
			else if (noom_strcmp(version_string, "53") == 0) params.lua_version = NOOM_VERSION_53;
			else if (noom_strcmp(version_string, "54") == 0) params.lua_version = NOOM_VERSION_54;
			else {
				err = "unknown lua version";
				goto die;
			}
			continue;
		}

		if (argv[i][1] == 'v') {
			puts(NOOM_VERSION_TEXT);
			return 0;
		}

		err = "unknown option";
		goto die;
	}
	if (params.lua_version != 0) {
		if (!params.do_i_already_know_what_to_do) {
			params.enter_repl = 1;
			params.do_i_already_know_what_to_do = 1;
		}
	}
	if (params.lua_version == 0) {
		params.lua_version = NOOM_VERSION_54;
	}
	if (!params.do_i_already_know_what_to_do) {
		err = "script not set";
		goto die;
	}
	if (params.script_exec || params.script_path) {
		if (params.script_exec) {
			return execute(params.script_exec, params.lua_version, argv[0], "(command line)");
		}
		char* code = read_file(params.script_path);
		if (code == 0) return 1;
		int offset = 0;
		if (code[0] == '#' && code[1] == '!')
			for (offset = 2; code[offset] && code[offset] != '\n'; offset++);
		const int e = execute(code + offset, params.lua_version, argv[0], params.script_path);
		noom_free(code);
		return e;
	}
	if (params.use_stdin) {
		char* code = read_stdin();
		if (code == 0) return 1;
		const int e = execute(code, params.lua_version, argv[0], "stdin");
		noom_free(code);
		return e;
	}
	if (params.enter_repl) {
		puts(NOOM_VERSION_TEXT);
		for (;;) {
			char code[4096];
			if (read_prompt(code, sizeof(code), "> ", 1)) return 0;
			execute(code, params.lua_version, 0, "(noom input)");
		}
	}
die:
	fprintf(stderr, "%s: %s\n"
	                "usage: %s [options] [script [args]]\n"
	                "Available options are:\n"
	                "  -                execute stdin\n"
	                "  -e stat          execute string 'stat'\n"
	                "  -v               show version\n"
	                "  -l [51|52|53|54] select lua version\n",
					argv[0], err, argv[0]);
	return 1;
}
