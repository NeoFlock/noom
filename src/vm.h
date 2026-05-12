#ifndef NOOM_VM
#define NOOM_VM

#include "types.h"

// Defines values, threads, bullshit, idc

typedef enum noomV_ObjTag {
	NOOMV_OSTR,
	NOOMV_OFUNC,
	NOOMV_OCLOSURE,
	NOOMV_OTABLE,
	NOOMV_OUSERDATA,
	NOOMV_OPOINTER,
} noomV_ObjTag;

typedef struct noomV_Object {
	noomV_ObjTag tag;
	bool marked;
	struct noomV_Object *next;
	struct noomV_Object *nextGray;
} noomV_Object;

typedef enum noomV_ValueTag {
	NOOMV_VNIL,
	NOOMV_VBOOL,
	NOOMV_VINT,
	NOOMV_VNUM,
	NOOMV_VLUSER,
	NOOMV_VCFUNC,
	NOOMV_VOBJ,
} noomV_ValueTag;

typedef struct noomV_Value {
	noomV_ValueTag tag;
	union {
		noom_bool_t boolean;
		noom_int_t integer;
		noom_float_t number;
		noomV_Object *obj;
	};
} noomV_Value;

typedef struct noomV_String {
	noomV_Object obj;
	noom_uint_t len;
	// Includes NUL-terminator!
	char data[];
} noomV_String;

typedef struct noomV_Table {
	noomV_Object obj;
	struct noomV_Table *meta;
	noom_uint_t entries;
	noomV_Value *entrydata;
} noomV_Table;

typedef struct noomV_Pointer {
	noomV_Object obj;
	noomV_Value value;
} noomV_Pointer;

typedef enum noomV_Opcode : unsigned char {
	NOOMV_INSTR_NOP = 0,
	NOOMV_INSTR_LOADC,
	NOOMV_INSTR_LOAD_NIL,
	NOOMV_INSTR_LOAD_TRUE,
	NOOMV_INSTR_LOAD_FALSE,
	NOOMV_INSTR_COPY,
	
	NOOMV_INSTR_JMP,
	NOOMV_INSTR_JC,


	NOOMV_INSTR_ADD,
	NOOMV_INSTR_SUB,
	NOOMV_INSTR_NEG,
	NOOMV_INSTR_MUL,
	NOOMV_INSTR_DIV,
	NOOMV_INSTR_MOD,
	NOOMV_INSTR_FLOOR_DIV,
	NOOMV_INSTR_POW,

	NOOMV_INSTR_EQ,
	NOOMV_INSTR_NEQ,
	NOOMV_INSTR_LT,
	NOOMV_INSTR_LTE,
	NOOMV_INSTR_GT,
	NOOMV_INSTR_GTE,

	NOOMV_INSTR_NOT,

	NOOMV_INSTR_BAND,
	NOOMV_INSTR_BOR,
	NOOMV_INSTR_BXOR,
	NOOMV_INSTR_BNOT,
	NOOMV_INSTR_LSHIFT,
	NOOMV_INSTR_RSHIFT,
	
	NOOMV_INSTR_CONCAT,
	NOOMV_INSTR_LEN,
	NOOMV_INSTR_GET,
	NOOMV_INSTR_GETI,
	NOOMV_INSTR_SET,
	NOOMV_INSTR_SETI,

#ifdef NOOM_USE_NFT
	NOOMV_INSTR_LOAD_NFT,
#endif

	NOOMV_INSTR_NOP2 = 0xff,
} noomV_Opcode;

typedef struct noomV_Inst {
	noomV_Opcode op;
	unsigned char a;
	union {
		struct {
			unsigned char b;
			unsigned char c;
		};
		short ss;
		unsigned short us;
	};
} noomV_Inst;

typedef struct noomV_UpvalDesc {
	char *name;
	unsigned char idx;
	noom_bool_t isStack;
} noomV_UpvalDesc;

typedef struct noomV_LocalDesc {
	char *name;
	unsigned char stackIdx;
	// offset of first instruction where local exists
	unsigned int pcStart;
	// offset of first instruction where local is dropped
	unsigned int pcEnd;
} noomV_LocalDesc;

typedef struct noomV_Function {
	noomV_Object obj;
	// source location string
	noomV_String *chunkname;
	noomV_Inst *code;
	noomV_Value *consts;
	noomV_Function **protos;
	noomV_UpvalDesc *upvals;
	noomV_LocalDesc *locals;
	unsigned int codesize;
	unsigned int linedefined;
	unsigned int lastlinedefined;
	unsigned char argc;
	unsigned char flags;
	unsigned char constsize;
	unsigned char protosize;
	unsigned char upvalsize;
	unsigned char localsize;
} noomV_Function;

#endif
