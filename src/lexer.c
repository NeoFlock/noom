#include "types.h"
#include "helper.h"
#include "lexer.h"

int noomL_isalpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int noomL_isnumber(char c) {
	return (c >= '0' && c <= '9');
}

int noomL_isalphanum(char c) {
	return noomL_isalpha(c) || noomL_isnumber(c);
}

int noomL_iswhitespace(char c) {
	return c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\v' || c == '\f';
}

noom_uint_t noomL_getsymbol(const char* s) { // TODO: maybe find some less shit crap holy crap
	if (noom_startswith(s, "...")) return 3;
	
	if (noom_startswith(s, "==")) return 2;
	if (noom_startswith(s, "~=")) return 2;
	if (noom_startswith(s, "<=")) return 2;
	if (noom_startswith(s, ">=")) return 2;
	if (noom_startswith(s, "..")) return 2;

	if (noom_startswith(s, "::")) return 2;
	
	if (noom_startswith(s, "//")) return 2;

	if (noom_startswith(s, ">>")) return 2;
	if (noom_startswith(s, "<<")) return 2;
	
	if (noom_startswith(s, "+")) return 1;
	if (noom_startswith(s, "-")) return 1;
	if (noom_startswith(s, "*")) return 1;
	if (noom_startswith(s, "/")) return 1;
	if (noom_startswith(s, "%")) return 1;
	if (noom_startswith(s, "^")) return 1;
	if (noom_startswith(s, "#")) return 1;
	if (noom_startswith(s, "<")) return 1;
	if (noom_startswith(s, ">")) return 1;
	
	if (noom_startswith(s, "=")) return 1;
	if (noom_startswith(s, ",")) return 1;
	
	if (noom_startswith(s, "(")) return 1;
	if (noom_startswith(s, ")")) return 1;
	if (noom_startswith(s, "{")) return 1;
	if (noom_startswith(s, "}")) return 1;
	if (noom_startswith(s, "[")) return 1;
	if (noom_startswith(s, "]")) return 1;

	if (noom_startswith(s, ":")) return 1;
	if (noom_startswith(s, ".")) return 1;
	
	if (noom_startswith(s, ";")) return 1;

	if (noom_startswith(s, "&")) return 1;
	if (noom_startswith(s, "|")) return 1;
	if (noom_startswith(s, "~")) return 1;

	return 0; // no symbol
}

noom_uint_t noomL_getnumber(const char* s) { // TODO: more number kinds idk
	// lazy af rn
	noom_uint_t len = 0;

	while (noomL_isnumber(s[len])) len++;

	return len;
}

int noomL_iskeyword(const char* s, noom_uint_t len) {
	if (noom_streql(s, len, "true", 4)) return 1;
	if (noom_streql(s, len, "false", 5)) return 1;
	if (noom_streql(s, len, "nil", 3)) return 1;
	
	if (noom_streql(s, len, "if", 2)) return 1;
	if (noom_streql(s, len, "then", 4)) return 1;
	if (noom_streql(s, len, "else", 4)) return 1;
	if (noom_streql(s, len, "elseif", 6)) return 1;
	
	if (noom_streql(s, len, "and", 3)) return 1;
	if (noom_streql(s, len, "or", 2)) return 1;
	if (noom_streql(s, len, "not", 3)) return 1;
	
	if (noom_streql(s, len, "local", 5)) return 1;
	
	if (noom_streql(s, len, "for", 3)) return 1;
	if (noom_streql(s, len, "function", 8)) return 1;
	if (noom_streql(s, len, "do", 2)) return 1;
	if (noom_streql(s, len, "until", 5)) return 1;
	if (noom_streql(s, len, "goto", 4)) return 1;
	if (noom_streql(s, len, "while", 5)) return 1;
	if (noom_streql(s, len, "repeat", 6)) return 1;
	if (noom_streql(s, len, "end", 3)) return 1;
	if (noom_streql(s, len, "in", 2)) return 1;
	if (noom_streql(s, len, "return", 6)) return 1;
	if (noom_streql(s, len, "break", 5)) return 1;

	return 0;
}

const char *noomL_formatTokenType(noomL_TokenType token_type) {
	switch (token_type) {
		case NOOML_TOKEN_EOF:
			return "EOF";
		case NOOML_TOKEN_KEYWORD:
			return "keyword";
		case NOOML_TOKEN_WHITESPACE:
			return "whitespace";
		case NOOML_TOKEN_IDENTIFIER:
			return "identifier";
		case NOOML_TOKEN_STRING:
			return "string";
		case NOOML_TOKEN_SYMBOL:
			return "symbol";
		case NOOML_TOKEN_NUMBER:
			return "number";
		default:
			return "unknown";
	}

}

noomL_ErrorType noomL_lex(const char* s, noom_uint_t start, noomL_Token* token, noom_LuaVersion version) {
	const char* str = s + start;

	if (str[0] == '\0') {
		token->type = NOOML_TOKEN_EOF;
		token->offset = start;
		token->length = 0;

		return NOOML_ERROR_NONE;
	}

	if (noomL_iswhitespace(str[0])) {
		token->type = NOOML_TOKEN_WHITESPACE; // TODO: maybe handle multiple at once? for now it should be okay
		token->offset = start;
		token->length = 1;

		return NOOML_ERROR_NONE;
	}

	if (str[0] == '_' || noomL_isalpha(str[0])) { // TODO: maybe abstract into function for "can start ident"?
		noom_uint_t len = 1;
		while (str[len] == '_' || noomL_isalphanum(str[len])) // same here
			len++;

		token->type = NOOML_TOKEN_IDENTIFIER;
		if (noomL_iskeyword(str, len)) token->type = NOOML_TOKEN_KEYWORD;
		
		token->offset = start;
		token->length = len;

		return NOOML_ERROR_NONE;
	}

	{
		noom_uint_t symbolLen = noomL_getsymbol(str);

		if (symbolLen) {
			token->type = NOOML_TOKEN_SYMBOL;
			token->offset = start;
			token->length = symbolLen;

			return NOOML_ERROR_NONE;
		}
	}

	{
		noom_uint_t numberLen = noomL_getnumber(str);

		if (numberLen) {
			token->type = NOOML_TOKEN_NUMBER;
			token->offset = start;
			token->length = numberLen;

			return NOOML_ERROR_NONE;
		}
	}

	// god damn it we errorrreed
	return NOOML_ERROR_UNKNOWN;
}

