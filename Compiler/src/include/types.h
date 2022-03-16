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

typedef enum {
		NOP = 0,
		MOV = 1,
		SUB = 2,
		ADD = 3,
		DIV = 4,
		MUL = 5,
		SHL = 6,
		SHR = 7,
		INT = 8,
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