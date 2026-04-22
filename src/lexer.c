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

int noomL_lower(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c - 'A' + 'a';
	}
	return c;
}

int noomL_ishex(char c) {
	return noomL_isnumber(c) || (noomL_lower(c) >= 'a' && noomL_lower(c) <= 'f');
}

noom_uint_t noomL_getsymbol(const char* s, noom_LuaVersion version) { // TODO: maybe find some less shit crap holy crap
	if (noom_startswith(s, "...")) return 3;
	
	if (noom_startswith(s, "==")) return 2;
	if (noom_startswith(s, "~=")) return 2;
	if (noom_startswith(s, "<=")) return 2;
	if (noom_startswith(s, ">=")) return 2;
	if (noom_startswith(s, "..")) return 2;

	if (version >= NOOM_VERSION_52) {
		if (noom_startswith(s, "::")) return 2;
	}

	if (version >= NOOM_VERSION_53) {
		if (noom_startswith(s, "//")) return 2;

		if (noom_startswith(s, ">>")) return 2;
		if (noom_startswith(s, "<<")) return 2;
	
		if (noom_startswith(s, "&")) return 1;
		if (noom_startswith(s, "|")) return 1;
		if (noom_startswith(s, "~")) return 1;
	}
	
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

	return 0; // no symbol
}

noom_uint_t noomL_getnumber(const char* s, noomL_ErrorType* error, noom_LuaVersion version) { // TODO: more number kinds idk
	noom_uint_t len = 0;
	
	if (s[0] == '0' && noomL_lower(s[1]) == 'x') {
		len = 2;

		while (noomL_ishex(s[len])) {
			len++;
		}

		if (version >= NOOM_VERSION_52) { // 5.2 added exponent and decimal to hex literals.

			if (s[len] == '.') { // decimals in hex. smh.
				len++;

				while (noomL_ishex(s[len])) {
					len++;
				}

				if (len == 3) { // only 0x. is a malformed number, even if followed by an exponent
					*error = NOOML_ERROR_MALFORMED_NUM;
					return 0;
				}
			}

			if (noomL_lower(s[len]) == 'p') {
				len++;

				// sign for exponent
				if (s[len] == '-' || s[len] == '+') len++;

				noom_uint_t slen = len;

				while (noomL_isnumber(s[len])) {
					len++;
				}

				if (slen == len) { // nothing after `p`
					*error = NOOML_ERROR_MALFORMED_NUM;
					return 0;
				}
			}
			
		}

		if (len == 2) { // nothing after the x; malformed number.
			*error = NOOML_ERROR_MALFORMED_NUM;
			return 0;
		}

		// check if identifier starter, if so, it's malformed (you can't do a=0xffl=2 or whatevs)
		if (version >= NOOM_VERSION_51) { // always true for now, but if 5.0 this shouldn't happen.
			if (noomL_isalpha(s[len]) || s[len] == '_') {
				*error = NOOML_ERROR_MALFORMED_NUM;
				return 0;
			}
		}
		
		return len;
	} else {
		while (noomL_isnumber(s[len])) { // int part
			len++;
		}

		if (s[len] == '.') { // it's-a me, decimal number
			len++;

			while (noomL_isnumber(s[len])) { // decimal numbering
				len++;
			}

			if (len == 1) { // only a . is an invalid number (it's a symbol instead!)
				// don't error; it's just a symbol, everything's okay.
				return 0;
			}
		}

		if (noomL_lower(s[len]) == 'e') { // exponent
			len++;

			// sign for exponent
			if (s[len] == '-' || s[len] == '+') len++;

			noom_uint_t slen = len;
					
			while (noomL_isnumber(s[len])) { // the exponent
				len++;
			}

			// exponent has no numbers in it, malformed
			if (slen == len) {
				*error = NOOML_ERROR_MALFORMED_NUM;
				return 0;
			}
		}

		// check if identifier starter, if so, it's malformed (you can't do a=53b=2 or whatevs)
		if (version >= NOOM_VERSION_51) { // always true for now, but if 5.0 this shouldn't happen.
			if (noomL_isalpha(s[len]) || s[len] == '_') {
				*error = NOOML_ERROR_MALFORMED_NUM;
				return 0;
			}
		}

		return len;
	}

	return 0;
}

noom_uint_t noomL_getcomment(const char* str, noomL_ErrorType* error) {
	if (str[0] == '-' && str[1] == '-') {
		noom_uint_t len = 2;
		noom_uint_t longb_len = 0;
	
		// check for long bracket
		int is_long = 0; // int for bools :fire:

		if (str[len] == '[') {
			len++;

			while (str[len] == '=') { longb_len++; len++; }

			if (str[len] == '[') {
				// yay long bracket!
				is_long = 1;
				len++;
			}
		}

		if (is_long) {

			// oh boy.

			while (1) {
				if (str[len] == ']') { // maybe this is it!
					len++;

					int success = 0;
					noom_uint_t spos = len; // after the ] intentionally
					noom_uint_t testlong = 0;

					while (str[len] == '=') { testlong++; len++; }

					if (str[len] == ']') { // actual long bracket! holy shit!
						len++;
						if (testlong == longb_len) {
							// we're done!
							success = 1;
						}
					}

					if (success) {
						return len;
					} else {
						len = spos;
					}
				} else if (str[len] == '\0') { // comment never finished
					*error = NOOML_ERROR_UNFINISHED_COMMENT;
					return 0;
				} else {
					len++; // just some character.
				}
			}
		
		} else {
			// reset to remove stuff, in case we hit like --[===hello, technically not required but a good idea
			len = 2;

			while (str[len] != '\0' && str[len] != '\n') len++;

			return len;
		}
	}

	return 0;
}

noom_uint_t noomL_getstring(const char* s, noomL_ErrorType* error, noom_LuaVersion version) {
	noom_uint_t len = 0;
	if (s[len] == '"' || s[len] == '\'') {
		char starter = s[len]; // either `'` or `"`
		
		len++; // double quoted string

		while (1) {
			if (s[len] == starter) {
				// it's over
				len++;
				return len; // found a whole string!
			} else if (s[len] == '\\') {
				len++; // oh boy!
				if (s[len] == '\\') {
					len++;
				} else if (s[len] == 'a') {
					len++;
				} else if (s[len] == 'b') {
					len++;
				} else if (s[len] == 'f') {
					len++;
				} else if (s[len] == 'n') {
					len++;
				} else if (s[len] == 'r') {
					len++;
				} else if (s[len] == 't') {
					len++;
				} else if (s[len] == 'v') {
					len++;
					
				// both string using single or double quote can have either escaped inside
				} else if (s[len] == '"') {
					len++;
				} else if (s[len] == '\'') {
					len++;
				} else if (s[len] == '\n') {
					len++;
				} else if (s[len] == '\r' && s[len+1] == '\n') { // fuck windows :fire:
					len += 2;
					
				} else if (noomL_isnumber(s[len])) {
					// fuckkkk
					noom_uint_t count = 0;
					for (noom_uint_t i = 0; i < 3; i++) {
						if (noomL_isnumber(s[len + i])) count++; else break;
					}
					if (count == 3) { // could be too big
						if ((s[len] > '2') || (s[len] == '2' && (s[len+1] > '5' || (s[len+1] == '5' && s[len+2] > '5')))) {
							// >255, i could also make it a number first but meh
							*error = NOOML_ERROR_DECIMAL_ESCAPE_TOO_BIG;
							return 0;
						}
					}
					// count can't be 0 because this if wouldn't run.
					len += count;
					
				} else if (s[len] == 'x' && version >= NOOM_VERSION_52) {
					len++;

					if ((!noomL_ishex(s[len])) || (!noomL_ishex(s[len+1]))) {
						*error = NOOML_ERROR_HEX_ESCAPE_INVALID;
						return 0;
					}
					len += 2;

				} else if (s[len] == 'z' && version >= NOOM_VERSION_52) {
					len++;
					while (noomL_iswhitespace(s[len])) len++;

				} else if (s[len] == 'u' && version >= NOOM_VERSION_53) {
					len++;
					if (s[len] != '{') {
						*error = NOOML_ERROR_UNICODE_ESCAPE_UNOPENED;
						return 0;
					}
					len++;

					while (s[len] == '0') len++; // remove leading zeroes.

					noom_uint_t hexlen = 0;
					while (noomL_ishex(s[len + hexlen])) hexlen++;

					// 5.3 allows <= 10ffff
					// whereas 5.4 allows <= 7fffffff
					// fuck my life
					if (version == NOOM_VERSION_53) {
						if (hexlen == 6) {
							if (s[len] > '1' || (s[len] == '1' && s[len+1] > '0')) {
								*error = NOOML_ERROR_UNICODE_ESCAPE_TOO_BIG;
								return 0;
							}
						} else if (hexlen > 6) {
							*error = NOOML_ERROR_UNICODE_ESCAPE_TOO_BIG;
							return 0;
						}
					} else if (version >= NOOM_VERSION_54) {
						if (hexlen == 8) {
							if (s[len] > '7') {
								*error = NOOML_ERROR_UNICODE_ESCAPE_TOO_BIG;
								return 0;
							}
						} else if (hexlen > 8) {
							*error = NOOML_ERROR_UNICODE_ESCAPE_TOO_BIG;
							return 0;
						}
					}
					len += hexlen;

					if (s[len] != '}') {
						*error = NOOML_ERROR_UNICODE_ESCAPE_UNCLOSED;
						return 0;
					}
					len++;

				} else if (s[len] == '\0') {
					// no. just leave it for the string to find afterward.
				} else {
					len++; // allow any random escape
				}
			} else if (s[len] == '\0') {
				*error = NOOML_ERROR_UNFINISHED_STRING;
				return 0;
			} else if (s[len] == '\n') {
				// unfinished because you can't have a newline in it
				*error = NOOML_ERROR_UNFINISHED_STRING;
				return 0;
			
			} else {
				len++; // anything else is just a thing in the string.
			}
		}
	} else if (s[len] == '[') { // potential multi-line string
		len++;

		noom_uint_t order = 0;
		int succ = 0;

		while (s[len] == '=') { order++; len++; }

		if (s[len] == '[') { len++; succ = 1; }

		if (succ) { // it is a multi-line string.
			while (1) {
				if (s[len] == ']') { // potential ender
					len++;
					noom_uint_t order2 = 0;
					noom_uint_t startp = len; // intentionally after the `]`

					while (s[len] == '=') { order2++; len++; }

					if (s[len] == ']' && order == order2) { // holy shit it's real
						len++;

						return len;
					} else {
						// nope.
						len = startp; // go back just in case like ]=]==]
					}
				
				} else if (s[len] == '\0') {
					*error = NOOML_ERROR_UNFINISHED_LONG_STRING;
					return 0;
				} else {
					len++;
				}
			}
		}
	}
	
	return 0;
}

int noomL_iskeyword(const char* s, noom_uint_t len, noom_LuaVersion version) {
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
	if (noom_streql(s, len, "while", 5)) return 1;
	if (noom_streql(s, len, "repeat", 6)) return 1;
	if (noom_streql(s, len, "end", 3)) return 1;
	if (noom_streql(s, len, "in", 2)) return 1;
	if (noom_streql(s, len, "return", 6)) return 1;
	if (noom_streql(s, len, "break", 5)) return 1;

	if (version >= NOOM_VERSION_52) {
		if (noom_streql(s, len, "goto", 4)) return 1;
	}
	
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
		case NOOML_TOKEN_COMMENT:
			return "comment";
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
		if (noomL_iskeyword(str, len, version)) token->type = NOOML_TOKEN_KEYWORD;
		
		token->offset = start;
		token->length = len;

		return NOOML_ERROR_NONE;
	}

	{ // must be above symbols because `.2` is a number
		noomL_ErrorType err = NOOML_ERROR_NONE;
		noom_uint_t numberLen = noomL_getnumber(str, &err, version);

		if (numberLen) {
			token->type = NOOML_TOKEN_NUMBER;
			token->offset = start;
			token->length = numberLen;

			return NOOML_ERROR_NONE;
		} else {
			if (err != NOOML_ERROR_NONE) return err;
		}
	}

	{
		noomL_ErrorType err = NOOML_ERROR_NONE;
		noom_uint_t commentLen = noomL_getcomment(str, &err);

		if (commentLen) {
			token->type = NOOML_TOKEN_COMMENT;
			token->offset = start;
			token->length = commentLen;

			return NOOML_ERROR_NONE;
		} else {
			if (err != NOOML_ERROR_NONE) return err;
		}
	}

	{
		noomL_ErrorType err = NOOML_ERROR_NONE;
		noom_uint_t stringLen = noomL_getstring(str, &err, version);

		if (stringLen) {
			token->type = NOOML_TOKEN_STRING;
			token->offset = start;
			token->length = stringLen;

			return NOOML_ERROR_NONE;
		} else {
			if (err != NOOML_ERROR_NONE) return err;
		}
	}
	
	{
		noom_uint_t symbolLen = noomL_getsymbol(str, version);

		if (symbolLen) {
			token->type = NOOML_TOKEN_SYMBOL;
			token->offset = start;
			token->length = symbolLen;

			return NOOML_ERROR_NONE;
		}
	}


	// god damn it we errorrreed
	return NOOML_ERROR_UNKNOWN;
}

