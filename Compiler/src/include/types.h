#ifndef TYPES_H
#define TYPES_H

// These are pointers
#define A 0x10 // (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)
#define B 0x20 // (AH = A & 0xF0) (AL = A & 0x0F) (ptr AX = A) (ptr AL = A - 0x08)
#define C 0x30
#define D 0x40
#define IP 0x50
#define SP 0x60
#define BP 0x70
#define I1 0x80
#define I2 0x90
#define I3 0x100
#define I4 0x110

#include <stdint.h>

static const char* OPERATION_T_NAMES[] = {"nop", "mov", "sub", "add", "div", "mul", "and", "or", "xor", "not", "shl", "shr", "int", "call", "jmp", "cmp", "je", "jne", "jz", "jnz", "jc", "jnc"};
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
		ENDFILE = -1
} OPERATION_T;

typedef struct {
	OPERATION_T operation;
	uint16_t value1;
	uint16_t value2;
} TOKEN_T;

struct LIST {
	TOKEN_T value;
	struct LIST* next;
};
typedef struct LIST LIST_T;

#endif