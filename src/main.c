#include <stdio.h> // for now
#include "lexer.h"

int main(int argc, char** argv) {
	// uhh uhhh uhhhhh
	const char* code = "local a = 5;";
	noom_uint_t pos = 0;

	noomL_Token token;
	while (1) {
		noomL_lex(code, pos, &token);

		printf("%d ", token.type);
		for (noom_uint_t i = 0; i < token.length; i++) putchar((code + token.offset)[i]);
		putchar('\n');

		pos += token.length;

		if (token.type == NOOML_TOKEN_EOF) break;
	}
	
	
	return 0;
}
