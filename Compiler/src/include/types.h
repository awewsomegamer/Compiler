#ifndef TYPES_H
#define TYPES_H

// These are pointers
// (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)
// (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)

#define DEFINE_INSTRUCTION(key, argc) {key, argc}

#include <stdint.h>

static const char* SIZE_T_NAMES[] = {"byte", "word", "dword"};
typedef enum{
	BYTE,
	WORD,
	DWORD,
	SIZE_T_MAX
} SIZE_T;

static const char* REGISTER_T_NAMES[] = {"ax", "bx", "cx", "dx", "ip", "sp", "bp", "i1", "i2", "i3", "i4"};

struct INSTRUCTION_INFORMATION {
	char* key;
	int argc;
};

typedef enum {
	NOP = 0,
	MOV,
	SUB,
	ADD,
	DIV,
	MUL,
	AND,
	OR,
	XOR,
	NOT,
	SHL,
	SHR,
	SIVTE,
	RIVTE,
	INT,
	CALL,
	JMP,
	CMP,
	JE,
	JNE,
	JG,
	JGE,
	JL,
	JLE,
	JZ,
	JNZ,
	JC,
	JNC,
	RET,
	IRET,
	PUSH,
	POP,
	INB,
	INW,
	IND,
	OUTB,
	OUTW,
	OUTD,
	DEFINITION_STRING,
	DEFINITION_BYTES,
	INCLUDE,
	INSTRUCTION_MAX,
	ENDFILE = -1
} OPERATION_T;


static const struct INSTRUCTION_INFORMATION INSTRUCTION_SET[] = {
	[NOP] 					= DEFINE_INSTRUCTION("nop",			0),
	[MOV] 					= DEFINE_INSTRUCTION("mov",			2),
	[SUB] 					= DEFINE_INSTRUCTION("sub",			2),
	[ADD] 					= DEFINE_INSTRUCTION("add", 		2),
	[DIV] 					= DEFINE_INSTRUCTION("div",			2),
	[MUL] 					= DEFINE_INSTRUCTION("mul",			2),
	[AND] 					= DEFINE_INSTRUCTION("and",			2),
	[OR]  					= DEFINE_INSTRUCTION("or", 			2),
	[XOR] 					= DEFINE_INSTRUCTION("xor",			2),
	[NOT] 					= DEFINE_INSTRUCTION("not",			1),
	[SHL] 					= DEFINE_INSTRUCTION("shl",			2),
	[SHR] 					= DEFINE_INSTRUCTION("shr",			2),
	[SIVTE] 				= DEFINE_INSTRUCTION("sivte",		2),
	[RIVTE] 				= DEFINE_INSTRUCTION("rivte",		2),
	[INT] 					= DEFINE_INSTRUCTION("int",			1),
	[CALL]  				= DEFINE_INSTRUCTION("call", 		1),
	[JMP] 					= DEFINE_INSTRUCTION("jmp",			1),
	[CMP] 					= DEFINE_INSTRUCTION("cmp",			2),
	[JE] 					= DEFINE_INSTRUCTION("je",			1),
	[JNE] 					= DEFINE_INSTRUCTION("jne",			1),
	[JG] 					= DEFINE_INSTRUCTION("jg", 			1),
	[JGE] 					= DEFINE_INSTRUCTION("jge",			1),
	[JL] 					= DEFINE_INSTRUCTION("jl",			1),
	[JLE] 					= DEFINE_INSTRUCTION("jle",			1),
	[JZ] 					= DEFINE_INSTRUCTION("jz",			1),
	[JNZ] 					= DEFINE_INSTRUCTION("jnz", 		1),
	[JC] 					= DEFINE_INSTRUCTION("jc",			1),
	[JNC] 					= DEFINE_INSTRUCTION("jnc",			1),
	[RET] 					= DEFINE_INSTRUCTION("ret",			0),
	[IRET] 					= DEFINE_INSTRUCTION("iret",		0),
	[PUSH]  				= DEFINE_INSTRUCTION("push", 		1),
	[POP] 					= DEFINE_INSTRUCTION("pop",			1),
	[INB] 					= DEFINE_INSTRUCTION("inb",			2),
	[INW] 					= DEFINE_INSTRUCTION("inw",			2),
	[IND] 					= DEFINE_INSTRUCTION("ind",			2),
	[OUTB]  				= DEFINE_INSTRUCTION("outb", 		2),
	[OUTW] 					= DEFINE_INSTRUCTION("outw",		2),
	[OUTD] 					= DEFINE_INSTRUCTION("outd",		2),
	[DEFINITION_STRING] 	= DEFINE_INSTRUCTION("ds",			3),
	[DEFINITION_BYTES] 		= DEFINE_INSTRUCTION("db",			3),
	[INCLUDE]				= DEFINE_INSTRUCTION("inc",			3)
};

// static const char* OPERATION_T_NAMES[] = {"nop", "mov", "sub", "add", 
// 										  "div", "mul", "and", "or", 
// 										  "xor", "not", "shl", "shr",
// 										  "sivte", "rivte", "int", "call", 
// 										  "jmp", "cmp", "je", "jne", "jg", 
// 										  "jge", "jl", "jle", "jz", "jnz", 
// 										  "jc", "jnc", "ret", "iret", "push", 
// 										  "pop", "inb", "inw", "ind", "outb", 
// 										  "outw", "outd", "ds", "db", "inc"};

// static const uint8_t OPERATION_T_ARGC[] = {
// 	0, 2, 2, 2,
// 	2, 2, 2, 2,
// 	2, 1, 2, 2,
// 	2, 2, 1, 1,
// 	1, 2, 1, 1, 1,
// 	1, 1, 1, 1, 1,
// 	1, 1, 0, 0, 1,
// 	1, 2, 2, 2, 2,
// 	2, 2, 3, 3, 0
// };


typedef struct {
	OPERATION_T operation;
	uint32_t value1;
	uint32_t value2;
	uint8_t* extra_bytes;
} TOKEN_T;

typedef struct{
	uint32_t address;
	char* name;
} LABEL_T;

typedef struct {
	char* id;
	int value;
} HASHMAP_ELEMENT_T;

#endif
