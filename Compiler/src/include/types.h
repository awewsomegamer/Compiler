#ifndef TYPES_H
#define TYPES_H

// These are pointers
// (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)
// (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)

#include <stdint.h>

static const char* SIZE_T_NAMES[] = {"byte", "word", "dword"};
typedef enum{
	BYTE,
	WORD,
	DWORD,
	SIZE_T_MAX
} SIZE_T;

static const char* REGISTER_T_NAMES[] = {"ax", "bx", "cx", "dx", "ip", "sp", "bp", "i1", "i2", "i3", "i4"};

static const char* OPERATION_T_NAMES[] = {"nop", "mov", "sub", "add", 
										  "div", "mul", "and", "or", 
										  "xor", "not", "shl", "shr",
										  "sivte", "rivte", "int", "call", 
										  "jmp", "cmp", "je", "jne", "jg", 
										  "jge", "jl", "jle", "jz", "jnz", 
										  "jc", "jnc", "ret", "inc", "push", 
										  "pop", "inb", "inw", "ind", "outb", 
										  "outw", "outd", "ds", "db"};

static const uint8_t OPERATION_T_ARGC[] = {
	0, 2, 2, 2,
	2, 2, 2, 2,
	2, 1, 2, 2,
	2, 2, 1, 1,
	1, 2, 1, 1, 1,
	1, 1, 1, 1, 1,
	1, 1, 0, 1, 1,
	1, 2, 2, 2, 2,
	2, 2, 3, 3
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
		INCLUDE,
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
		ENDFILE = -1
} OPERATION_T;

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
