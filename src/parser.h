#include "types.h"
#include "lexer.h"

typedef enum noomP_NodeType {
	NOOMP_NODE_PROGRAM,
	NOOMP_NODE_VARNAME,
	NOOMP_NODE_VARIABLE,
	NOOMP_NODE_NUMBERLITERAL,
	NOOMP_NODE_LOCALDECLARATION,
	NOOMP_NODE_IFSTATEMENT,
	NOOMP_NODE_BLOCK,

	NOOMP_NODE_UNARYOPERATOR,
	NOOMP_NODE_BINARYOPERATOR
} noomP_NodeType;

typedef struct noomP_Node {
	noomP_NodeType type;

	noom_uint_t source_offset;
	
	noom_uint_t subnodec;
	noom_uint_t subnode_cap;
	struct noomP_Node** subnodes;

	struct noomP_Node* previous_node;
} noomP_Node;

typedef struct noomP_Parser { // todo: track location in code with line/column?
	const char* code;
	const char* filename;
	noom_uint_t lex_offset;

	noomP_Node* last_node;
} noomP_Parser;

int noomP_peek(noomP_Parser* parser, noomL_Token* token);
void noomP_skip(noomP_Parser* parser, noomL_Token* token);

noomP_Node* noomP_allocNode(noomP_Parser* parser);

noomP_Node* noomP_parseStatement(noomP_Parser* parser);
noomP_Node* noomP_parseExpression(noomP_Parser* parser);

int noomP_parse(const char* code, const char* filename, noomP_Node** outpointer, noomP_Node** last_node);

int noomP_initParser(noomP_Parser* parser, const char* code, const char* filename);

