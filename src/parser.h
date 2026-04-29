#include "types.h"
#include "lexer.h"

typedef enum noomP_NodeType {
	NOOMP_NODE_PROGRAM,
	
	NOOMP_NODE_VARNAME,
	
	NOOMP_NODE_LOCALDECLARATION,
	NOOMP_NODE_IFSTATEMENT,
	NOOMP_NODE_WHILELOOP,
	NOOMP_NODE_FORLOOP,
	NOOMP_NODE_FORLOOPIN,
	NOOMP_NODE_BLOCK,

	NOOMP_NODE_ATTRIBUTE,

	NOOMP_NODE_BREAK,
	
	NOOMP_NODE_VARIABLE,
	NOOMP_NODE_NUMBERLITERAL,
	NOOMP_NODE_BOOLEANLITERAL,
	NOOMP_NODE_NILLITERAL,
	NOOMP_NODE_STRINGLITERAL,

	NOOMP_NODE_UNARYOPERATOR,
	NOOMP_NODE_BINARYOPERATOR,

	NOOMP_NODE_GETFIELD,
	NOOMP_NODE_INDEX,
	NOOMP_NODE_CALL,
	NOOMP_NODE_METHODCALL,

	NOOMP_NODE_LAMBDAFUNCTIONLITERAL,
	NOOMP_NODE_FUNCTIONDECLARATION,
	NOOMP_NODE_LOCALFUNCTIONDECLARATION,
	NOOMP_NODE_FUNCTIONPARAMETERS,
	NOOMP_NODE_FUNCTIONNAME,

	NOOMP_NODE_RETURN,

	NOOMP_NODE_FIELDNAME,
	NOOMP_NODE_METHODNAME, // only used in function decl as of right now.

	NOOMP_NODE_VARARG,

	NOOMP_NODE_PARENTHESIZED,

	NOOMP_NODE_ASSIGNMENT,
	NOOMP_NODE_ASSIGNPLACE,

	NOOMP_NODE_NCOUNT,
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
	noom_LuaVersion version;
	
	const char* code;
	const char* filename;
	noom_uint_t lex_offset;

	noomP_Node* last_node;
} noomP_Parser;

const char *noomP_formatNodeType(noomP_NodeType node_type);

int noomP_peek(noomP_Parser* parser, noomL_Token* token);
void noomP_skip(noomP_Parser* parser, noomL_Token* token);

noomP_Node* noomP_allocNode(noomP_Parser* parser);

noomP_Node* noomP_parseFunctionParameters(noomP_Parser* parser);
noomP_Node* noomP_parseBlock(noomP_Parser* parser);

noomP_Node* noomP_parseExpression(noomP_Parser* parser);
noomP_Node* noomP_parseStatement(noomP_Parser* parser);

int noomP_parse(const char* code, const char* filename, noom_LuaVersion version, noomP_Node** outpointer, noomP_Node** last_node);

int noomP_initParser(noomP_Parser* parser, const char* code, const char* filename, noom_LuaVersion version);

