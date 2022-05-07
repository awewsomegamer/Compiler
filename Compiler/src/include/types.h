#ifndef TYPES_H
#define TYPES_H

// These are pointers
// (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)
// (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)

#include <stdint.h>

static const char* REGISTER_T_NAMES[] = {"ax", "al", "ah", "bx", "bl", "bh", "cx", "cl", "ch","dx", "dl", "dh", "ip", "sp", "bp", "i1", "i2", "i3", "i4"};
// typedef enum {
// 	A = 0x10,
// 	B = 0x20,
// 	C = 0x30,
// 	D = 0x40,
// 	IP = 0x50,
// 	SP = 0x60,
// 	BP = 0x70,
// 	I1 = 0x80,
// 	I2 = 0x90,
// 	I3 = 0x100,
// 	I4 = 0x110
// } REGISTER_T;

static const char* OPERATION_T_NAMES[] = {"nop", "mov", "sub", "add", "div", "mul", "and", "or", "xor", "not", "shl", "shr", "int", "call", "jmp", "cmp", "je", "jne", "jg", "jge", "jl", "jle", "ret"};
typedef enum {
		NOP = 0,
		MOV = 1,
		SUB = 2,
		ADD = 3,
		DIV = 4,
		MUL = 5,
		AND = 6,
		OR = 7,
		XOR = 8,
		NOT = 9,
		SHL = 10,
		SHR = 11,
		INT = 12,
		CALL = 13,
		JMP = 14,
		CMP = 15,
		JE = 16,
		JNE = 17,
		JZ = 18,
		JNZ = 19,
		JC = 20,
		JNC = 21,
		RET = 22,
		ENDFILE = -1,
		DEFINITION_STRING = -2,
		DEFINITION_BYTES = -3
} OPERATION_T;

typedef struct {
	OPERATION_T operation;
	uint32_t value1;
	uint32_t value2;
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
