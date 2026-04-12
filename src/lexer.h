#include "types.h"

typedef enum noomL_TokenType {
	NOOML_TOKEN_EOF = 0,
	NOOML_TOKEN_KEYWORD,
	NOOML_TOKEN_WHITESPACE,
	NOOML_TOKEN_IDENTIFIER,
	NOOML_TOKEN_STRING,
	NOOML_TOKEN_SYMBOL,
	NOOML_TOKEN_NUMBER,

	NOOML_TOKEN_TCOUNT,
} noomL_TokenType;

typedef enum noomL_ErrorType {
	NOOML_ERROR_NONE = 0,
	NOOML_ERROR_UNKNOWN = 1,
} noomL_ErrorType;

typedef struct noomL_Token {
	noomL_TokenType type;
	noom_uint_t offset;
	noom_uint_t length;
} noomL_Token;


int noomL_isalpha(char c);
int noomL_isnumber(char c);
int noomL_isalphanum(char c);
int noomL_iswhitespace(char c);

noom_uint_t noomL_getsymbol(const char* s);
noom_uint_t noomL_getnumber(const char* s);

noomL_ErrorType noomL_lex(const char* s, noom_uint_t start, noomL_Token* token); // TODO: add more error data

