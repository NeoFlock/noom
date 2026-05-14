#ifndef NOOM_VM
#define NOOM_VM

#include "types.h"
#include "noom.h"

#ifndef NOOM_MAXSTACK
#define NOOM_MAXSTACK 4096
#endif

#ifndef NOOM_MAXCALL
#define NOOM_MAXCALL (NOOM_MAXSTACK/32)
#endif

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

typedef enum noomV_ValueTag : unsigned char {
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
	// for stack slots
	bool autoclose;
	// pointer to value
	bool isptr;
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
	noom_uint_t len;
	noomV_Value *entrydata;
} noomV_Table;

typedef struct noomV_Pointer {
	noomV_Object obj;
	noomV_Value value;
} noomV_Pointer;

typedef enum noomV_Opcode : unsigned char {
	NOOMV_INSTR_NOP = 0,
	// A = consts[uD]
	NOOMV_INSTR_LOADC,
	// load from A an array of uD nils
	NOOMV_INSTR_LOAD_NILARR,
	// load from A an array of uD/2 booleans, stored in uD&1
	NOOMV_INSTR_LOAD_BOOLARR,
	// A = B
	NOOMV_INSTR_COPY,

	// pc = uD
	NOOMV_INSTR_JMP,
	// if(A) pc = uD
	NOOMV_INSTR_JC,
	// if(not A) pc = uD
	NOOMV_INSTR_JNC,

	// A = B + C
	NOOMV_INSTR_ADD,
	// A = B - C
	NOOMV_INSTR_SUB,
	// A = -B
	NOOMV_INSTR_NEG,
	// A = B * C
	NOOMV_INSTR_MUL,
	// A = B / C
	NOOMV_INSTR_DIV,
	// A = B % C
	NOOMV_INSTR_MOD,
	// A = B // C
	NOOMV_INSTR_FLOOR_DIV,
	// A = B ^ C
	NOOMV_INSTR_POW,

	// A = B == C
	NOOMV_INSTR_EQ,
	// A = B ~= C
	NOOMV_INSTR_NEQ,
	// A = B < C
	NOOMV_INSTR_LT,
	// A = B <= C
	NOOMV_INSTR_LTE,
	// A = B > C
	NOOMV_INSTR_GT,
	// A = B >= C
	NOOMV_INSTR_GTE,

	// A = not B
	NOOMV_INSTR_NOT,

	// A = B & C
	NOOMV_INSTR_BAND,
	// A = B | C
	NOOMV_INSTR_BOR,
	// A = B ~ C
	NOOMV_INSTR_BXOR,
	// A = ~B
	NOOMV_INSTR_BNOT,
	// A = B << C
	NOOMV_INSTR_LSHIFT,
	// A = B >> C
	NOOMV_INSTR_RSHIFT,

	// A = B .. C
	NOOMV_INSTR_CONCAT,
	// A = #B
	NOOMV_INSTR_LEN,
	// A = B[C]
	NOOMV_INSTR_GET,
	// A[B] = C
	NOOMV_INSTR_SET,

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
	// very size limited
	//       ░░░░░░░░░                                                         ░ ▒░░▒    ▒░   ░░  ░       
	//       ░░░░░░░░░                                                         █░█░▓▓   ░█▒████▒░ █░░████▓
	//       ░░░░░░░░░                                                        ▓████████░█░▒████▓░████  ░█░
	//       ░░░░░░░░░           ░░░                                         ░█▓░█▒█░█░▒▒█▓████▒ ░█▒█ ░█░░
	//       ░░░░░░░░░         ░░░░░                                         ░█▓▓███▒█  ▓▓▒▒▒▓▓▒ ▒█▒█▓██▓▓
	//       ░░░░░░░░░           ░░                                          ░▓▓███▓█▒░░█▒▓█▓██▓ ▒▓█▓▓▓█▒▒
	//       ░░░░░░░░░░          ░░                                           ▒▓█░█▒█░ ░▓▓██████░░██░░▒█  
	//       ░░░░░░░░░░    ░░░░  ░░                                           ▓▓█▒█▓█▒░ ▒▒▒█▒░█░  ███░░█  
	//       ░░░░░░░░░░          ░░                                           ▓▒▓███░█▒ ▒░░░▓▓█░░██░▓▒██  
	//       ░░░░░░░░░░          ░░        ░                                  ░░  ░░ ░░ ░░   ▒░ ░░░  ░░   
	//       ░░░░░░░░░░  ░       ░░░       ░                                    ░ ░                  ░    
	//       ░░░░░░░░░░    ░░░░  ░░        ░                                                              
	// ░     ░░░░░░░░░     ░░░░                      ░         ░  ░         ░░     ░░░   ░░░ ░    ░░      
	//       ░░░░░░░░░        ░             █████  ▒████  ░▓███▓░ ░████░ ░████▓  ░████░ ░▓███▓  ░████░    
	//░░     ░░░░░░░░░                      █▓▒██░░██▓██▒░░██▒██░░██▒▓██░▒██▒██  ██▓▓██ ░██▒██░ ██▒▓██    
	//░░    ░░░░░░░░░░                         ██░▒█▓ ░██░██▒ ▒█▒░██ ░██ ██░ ▓█▒░██ ░██░██░ ▓█▒░██  ██░   
	//     ░░░░░░░░░░░                      ░▓███░▒█▓  ██░██░ ▒██▒██ ░██░██  ▒█▒░██ ░██░██░ ▒█▓▒█▓ ░██░   
	//      ░░░░░░░░░░        ░     ░▒███░  ▓████░▓█▒ ░██░██░ ▒██▒█▓ ░██░██ ░▒█▒░██ ░██░██░ ░██▒█▓ ░██░░  
	//       ░░░░░░░░░     ░░ ░              ░░██▓▓█▓░░██ ██░ ▒█▓▒█▓  ██░██░ ▒█▒░██ ░██░██░ ░██▒█▓ ░██░░  
	//      ░░░░░░░░░░   ░░░░               ░ ░███▒██ ░██ ██▒ ▓█▒▒██ ░██░██░ ██░░██░░██ ██▒ ▓█░░██ ░██    
	//       ░░░░░░░░░   ░░░               ░█████▒░█████▒▒▓█████ ░█████▒ ▒█████▓██████▓ ▒██▓██░ █████▒    
	//       ░░░░░░░░░   ░░                ░████▒ ░░███▒░█▒▒███░  ░███▓   ▓███░▓█░███▓░  ▒███░  ░███▒     
	//       ░░░░░░░░░░                                 ░█░    ░   ░           ▒▒          ░              
	//       ░░░░░░░░░                                   ░                                           ░    
	//       ░░░░░░░░░   ░░░░                                                                             
	//       ░░░░░░░░░   ░░░░░                     ░░                 ░          ░                 ░    ░ 
	//       ░░░░░░░░░   ░░   ░░▒░░               ░▓▒       ░█▓       ░                            ░    ░ 
	//       ░░░░░░░░░   ░░  ▒█████░              ░██       ░██     ░                               ░     
	//      ░░░░░░░░░░░░░    ██▒▒███   ░░   ░ ░ ░░ ░░       ░██                                           
	//   ░  ░░░░░░░░░░░░░░░  ██░ ░▓▓ ░████  ░████░░█▓ ▒███▓░░██                                           
	//  ░░░░░░░░░░░░░░░░░░░░░▓███░░ ░██████░██▓██▓▒██░██▓██▒▒██░                                          
	// ░░░░░░░░░░░░░░░░░░░░   ░████░░█▓  ██░██  ▓▓░██ ░░▒██▓▒██░     ░                                    
	// ░░░░░░░░░░░░░░░░░░░   ░░░░███▒█▓  ██▒██   ░░██░█████▓▒██░    ░░         ░                          
	//  ░░░░░░░░░░░░░░░░░░  ░██▒ ░██▒██░░██░██ ░█▓░██░██░▓█▓░██░    ░          ██▓████████▓▒██▒           
	//   ░░░░░░░░░░░░░░░░░  ░██████▓ █████▓ █████▓▒██░█████▓░██░    ░          ░░░███████████▓█▓          
	//    ░░░░░░░░░░░░░░░░░░ ░▓███▒  ░███▒  ░▓██▓ ░█▓ ▒██▓█▓░██░                 ░████████████▓█░         
	//    ░░░░░░░░░░░░░░░░░  ░                             ░                        ░█████████▓██         
	//    ░░░░░░░░░░░░░░ ░░                                                           ▒███████▓█▓         
	//   ░ ░░░░░░░░░░░░░░   ░░░░                                                      ██▓██▓▓▓███   ░     
	//   ░░░░░░░░░░░░░░░     ░░           ░                                          ░███████████         
	//     ░░░░░░░░░░░░    ░░                        ░                               ████████████    ▒    
	//     ░░░░░░░░░░░░       ░░▒▒░░         ░      ░██░▓█░                    ░   ░░████ ▒██████    █    
	//      ░░░░░░░░░░░       ▓█████░               ░██ ██░░█▓░                     ▓████  ▓█████   ░█  ▒▒
	//      ░░░░░░░░░░░      ▒██░░██▒░░░░░ ░░░░░ ░░░░██  ░░░██░                     █████  ▒█████   ▒█ ▒█░
	//    ░░░░░░░░░░░░░      ██▒  ░█▓░████░████░ ░█████ ██▒████                     █████  ▓████▒   ████░ 
	//    ░░░░░░░░░░░░       ██░     ▒███▓██▓▓██ ██▒▒██ ██░▒██░                     ████▒  █████ ▒████████
	//      ░░░░░░░░░░       ██░  ░  ▒██░ ██▒▒██░██ ░██ ██ ░██░                    ░████  ░█████   ▓██▓▒░░
	//      ░░░░░░░░░░       ██▒  ░▒░▒██ ░██████▒██ ░██░██░░██                     ▒███▒  █████▒          
	//   ░   ░░░░░░░░░       ▓█▓░ ▓█▓▒██ ░██░ ░░░██ ░██ ██░░██░                    ░███░░░█████           
	//       ░░░░░░░░░        ██████ ▒█▓  ▓█████ ██████ ██░░███░                   ░██▒  █████▒           
	//       ░░░░░░░░░        ░▓███░░░█▓ ░░▒███░░░██▓█▓ ▓█░ ▒█▓░                    ▓▓  ░█████            
	//       ░░░░░░░░░         ░░░░          ░░         ░░ ░ ░                      ▓▒  ▓████░            
	//       ░░░░░░░░            ░░          ░░      ░                              ▒▓  ████▒             
	//       ░ ░░░░░ ░                 ░                                             ▒░▓███▒              
	//         ░░░░             ░                                                    ░▓▓██                
	//          ░░░                                                                                       
	//          ░░░                                                                                       
	//          ░░                                                                                        	
	unsigned char argc;
	unsigned char flags;
	unsigned short constsize;
	unsigned char protosize;
	unsigned char upvalsize;
	unsigned char localsize;
} noomV_Function;

typedef struct noomV_CallFrame {
	// stack index of function
	unsigned int funcIdx;
	bool isC;
	union {
		struct {
			// 
			unsigned int pc;
			// amount of varargs
			unsigned int varargc;
		} lua;
		struct {
			noom_KFunction *resumeFunc;
			void *resumeCtx;
		} c;
	};
} noomV_CallFrame;

typedef struct noomV_Thread {
	noomV_Object obj;
	unsigned int stacklen, calldepth;
	unsigned int stackcap, callcap;
	// can have pointers!
	noomV_Value *stack;
	noomV_CallFrame *calls;
	struct noomV_Thread *resumedBy;
	struct noomV_Thread *resuming;
} noomV_Thread;

struct noom_LuaVM {
	noomV_Object *heap;
	noomV_Object *graySet;
	noomV_Table *globals;
	noomV_Table *registry;
	noomV_Thread *mainThread;
	noomV_Thread *currentThread;
};

#endif
