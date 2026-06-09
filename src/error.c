#include "error.h"
#include "helper.h"

noom_uint_t noom_format_error(const noomP_Parser* parser, const char* program_name, char* buffer, noom_uint_t buffer_size) {
	struct noom_error {
		const char* s;
		int near; // 0:none 1: near '%s'\n 2: '%s'\n
	};

	static const struct noom_error parser_errors[] = {
		[NOOMP_ERROR_NONE] = {0, 0},

		[NOOMP_ERROR_OOM] = {"Whoops! Out of memory :(\n", 0},
		[NOOMP_ERROR_EXPECTED_LCURLY] = {"expected '{'", 1},
		[NOOMP_ERROR_EXPECTED_RCURLY] = {"expected '}' to close table literal", 1},
		[NOOMP_ERROR_EXPECTED_RBRACKET_AFTER_KEY] = {"expected ']' to close computed key", 1},
		[NOOMP_ERROR_EXPECTED_EQUALS_AFTER_KEY] = {"expected '=' after table key", 1},
		[NOOMP_ERROR_EXPECTED_FIELD_IDENTIFIER] = {"expected identifier after '.' for field access", 1},
		[NOOMP_ERROR_EXPECTED_METHOD_CALL] = {"expected identifier after ':' for method call", 1},
		[NOOMP_ERROR_EXPECTED_RBRACKET_AFTER_INDEX] = {"expected ']' after index expression", 1},
		[NOOMP_ERROR_EXPECTED_RPAREN_AFTER_EXPRESSION] = {"expected ')' after expression", 1},
		[NOOMP_ERROR_EXPECTED_LPAREN_FOR_PARAMETERS] = {"expected '(' for function parameters", 1},
		[NOOMP_ERROR_EXPECTED_RPAREN_FOR_PARAMETERS] = {"expected ')' for function parameters", 1},
		[NOOMP_ERROR_EXPECTED_IDENTIFIER_AFTER_LOCAL_FUNCTION] = {"expected identifier after 'local function'\n", 0},
		[NOOMP_ERROR_EXPECTED_IDENTIFIER_AFTER_FUNCTION] = {"expected identifier after 'function'\n", 0},
		[NOOMP_ERROR_EXPECTED_END_AFTER_LOCAL_FUNCTION] = {"expected 'end' to close local function declaration", 1},
		[NOOMP_ERROR_EXPECTED_END_AFTER_FUNCTION] = {"expected 'end' to close function declaration", 1},
		[NOOMP_ERROR_EXPECTED_IDENTIFIER_AFTER_LOCAL] = {"expected identifier after 'local'\n", 0},
		[NOOMP_ERROR_EXPECTED_IDENTIFIER_AFTER_LANGLE] = {"expected identifier after '<' for attribute\n", 0},
		[NOOMP_ERROR_EXPECTED_RANGLE_TO_CLOSE_ATTRIBUTE] = {"expected '>' to close attribute after identifier", 1},
		[NOOMP_ERROR_EXPECTED_THEN_AFTER_EXPRESSION] = {"expected 'then' after expression", 1},
		[NOOMP_ERROR_EXPECTED_ELSE_ELSEIF_OR_END] = {"expected 'else', 'elseif', or 'end'", 1},
		[NOOMP_ERROR_EXPECTED_END_AFTER_IF] = {"expected 'end' to close if statement", 1},
		[NOOMP_ERROR_EXPECTED_DO_AFTER_EXPRESSION] = {"expected 'do' after expression", 1},
		[NOOMP_ERROR_EXPECTED_END_AFTER_WHILE] = {"expected 'end' to close while statement", 1},
		[NOOMP_ERROR_EXPECTED_END_AFTER_FOR] = {"expected 'end' to close for statement", 1},
		[NOOMP_ERROR_EXPECTED_END_AFTER_DO] = {"expected 'end' to close do statement", 1},
		[NOOMP_ERROR_EXPECTED_IDENTIFIER_AFTER_FOR] = {"expected identifier after 'for'\n", 1},
		[NOOMP_ERROR_EXPECTED_VARIABLE_AFTER_COMMA_IN_FOR] = {"expected variable name after ',' in for loop\n", 0},
		[NOOMP_ERROR_EXPECTED_IN] = {"expected 'in'", 1},
		[NOOMP_ERROR_EXPECTED_UNTIL] = {"expected 'until' to close repeat expression", 1},
		[NOOMP_ERROR_EXPECTED_IDENTIFIER_AFTER_GOTO] = {"expected identifier after goto\n", 0},
		[NOOMP_ERROR_EXPECTED_COLONCOLON] = {"expected :: to end label identifier", 1},
		[NOOMP_ERROR_INVALID_STATEMENT] = {"expected statement, got", 2},
		// I want someone smarter than me [tema5002] to give these a proper description
		// ^ alrighty then
		[NOOMP_ERROR_EXPECTED_ASSIGNABLE] = {"expected assignable expression after comma in assignment", 1},
		[NOOMP_ERROR_NOT_ASSIGNABLE] = {"expression in assignment is not assignable", 1},
		[NOOMP_ERROR_EXPECTED_COMMA_OR_EQUAL_IN_ASSIGNMENT] = {"expected a comma or equals after assignable in assignment", 1},
		[NOOMP_ERROR_EXPRESSION_NOT_STATEMENT] = {"loose expression is not a valid statement", 1},
		[NOOMP_ERROR_FAKEATTRIBUTE] = {"invalid attribute", 2},
		[NOOMP_ERROR_RETURN_NOT_END] = {"'return' must be the last statement in a block\n", 0},
		[NOOMP_ERROR_FOR_WRONG_AMOUNT] = {"'for' initializer must have 2 or 3 expressions\n", 0}
	};

	static const struct noom_error lexer_errors[] = {
		[NOOML_ERROR_NONE] = {0, 0},

		[NOOML_ERROR_UNKNOWN] = {"unknown token", 1},
		[NOOML_ERROR_MALFORMED_NUM] = {"malformed number", 1},
		[NOOML_ERROR_UNFINISHED_COMMENT] = {"unfinished comment", 1},
		[NOOML_ERROR_UNFINISHED_STRING] = {"unfinished string", 1},
		[NOOML_ERROR_UNFINISHED_LONG_STRING] = {"unfinished long string", 1},
		[NOOML_ERROR_DECIMAL_ESCAPE_TOO_BIG] = {"decimal escape sequence too big (max 255)", 1},
		[NOOML_ERROR_HEX_ESCAPE_INVALID] = {"invalid hexadecimal escape sequence", 1},
		[NOOML_ERROR_UNICODE_ESCAPE_UNOPENED] = {"expected '{' after '\\u' for Unicode escape", 1},
		[NOOML_ERROR_UNICODE_ESCAPE_UNCLOSED] = {"expected '}' to close Unicode escape sequence", 1},
		[NOOML_ERROR_UNICODE_ESCAPE_TOO_BIG] = {"Unicode escape sequence exceeds maximum (0x10FFFF)", 1},
	};

	noomP_Error base_err = parser->error_state;
	noom_uint_t lexer_code = 0;
	noom_uint_t pos = 0;

	if (parser->error_state & NOOMP_ERROR_LEXER) {
		base_err = NOOMP_ERROR_LEXER;
		lexer_code = parser->error_state & ~NOOMP_ERROR_LEXER;
	}

	struct noom_error err = (base_err == NOOMP_ERROR_LEXER) ? lexer_errors[lexer_code] : parser_errors[base_err];

	noom_uint_t row = 1, column = 1;
	for (noom_uint_t i = 0; i < parser->lex_offset; i++) {
		if (parser->code[i] == '\n') {
			row++;
			column = 1;
		}
		else {
			column++;
		}
	}

	if (buffer == 0) {
		noom_uint_t linedig = 0;
		for (noom_uint_t n = row; n; n /= 10, linedig++);

		noom_uint_t space = 0;

		if (program_name) space +=
			noom_strlen(program_name) +
			sizeof(": ") - 1;
		
		space += 
			noom_strlen(parser->filename) +
			sizeof(":") - 1 +
			linedig +
			sizeof(":") - 1 +
			noom_strlen(err.s) +
			+ 1; // \0;

		if (err.near) space += 
			(err.near == 1 ? sizeof(" near") - 1 : 0) +
			sizeof(" '") - 1 +
			parser->last_token_length +
			sizeof("'\n") - 1;

		return space;
	}

	if (program_name) {
		noom_safe_strcpy(buffer, &pos, buffer_size, program_name);
		noom_safe_strcpy(buffer, &pos, buffer_size, ": ");
	}
	noom_safe_strcpy(buffer, &pos, buffer_size, parser->filename);
	noom_safe_strcpy(buffer, &pos, buffer_size, ":");

	char num_buf[20];
	noom_uint_t num_len = 0;
	
	if (row == 0) {
		num_buf[num_len++] = '0';
	} else {
		noom_uint_t temp = row;
		noom_uint_t divisor = 1;
		while (temp / divisor >= 10) divisor *= 10;
		while (divisor > 0) {
			num_buf[num_len++] = '0' + (temp / divisor);
			temp %= divisor;
			divisor /= 10;
		}
	}

	for (noom_uint_t i = 0; i < num_len && pos < buffer_size - 1; i++) {
		buffer[pos++] = num_buf[i];
	}

	noom_safe_strcpy(buffer, &pos, buffer_size, ":");
	noom_safe_strcpy(buffer, &pos, buffer_size, err.s);
	if (err.near) {
		if (err.near == 1) {
			noom_safe_strcpy(buffer, &pos, buffer_size, " near");
		}
		noom_safe_strcpy(buffer, &pos, buffer_size, " '");
		for (noom_uint_t i = 0; i < parser->last_token_length && pos < buffer_size - 1; i++) {
			buffer[pos++] = parser->code[parser->last_token_offset + i];
		}
		noom_safe_strcpy(buffer, &pos, buffer_size, "'\n");
	}
	
	if (pos < buffer_size) buffer[pos] = '\0';
	return pos;
}
