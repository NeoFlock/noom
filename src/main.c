#include <stdio.h> // for now
#include "helper.h"
#include "error.h"
#include "noom.h"
#include "compiler.h"
#include "vm.h"

volatile const char wawa[] = "if you are reading this with either the `strings` utility or through a hex viewer please know that you can instead just check out the source code on gitea: https://gitea.codersquack.nl/NeoFlock/noom";

int execute(const char* code, noom_LuaVersion version, const char* program_name, const char* filename) {
	noom_LuaVM* vm = noom_createVM(version);
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
// ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡘⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
	(vm, code, filename, program_name);
	
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
		bool enter_repl;
		bool use_stdin;
		const char* script_exec;
		const char* script_path;
		bool do_i_already_know_what_to_do;
		noom_LuaVersion lua_version;
	} params = {0};

	if (argc < 2) {
		params.enter_repl = true;
		params.do_i_already_know_what_to_do = true;
	}

	for (int i = 1; i < argc; i++) {
		if (noom_strcmp(argv[i], "-") == 0) {
			params.use_stdin = true;
			params.do_i_already_know_what_to_do = true;
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
		params.lua_version = NOOM_VERSION_51;
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
