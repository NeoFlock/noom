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
	NOOMV_NOP,
	// TODO: rest of ops
} noomV_Opcode;

typedef struct noomV_Inst {
	noomV_Opcode op;
	unsigned char A;
	union {
		struct {
			unsigned char B;
			unsigned char C;
		};
		short sD;
		unsigned short uD;
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
