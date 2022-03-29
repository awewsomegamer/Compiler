#ifndef LEXER_H
#define LEXER_H

#include <types.h>
#include <stddef.h>

TOKEN_T tokenize(char* line, HASHMAP_ELEMENT_T label_map[]);

#endif