#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <string.h>
#include <types.h>

bool startsWith(const char* str, const char* lookfor);
bool containsCharacter(char* line, char c);
bool endsWith(char* line, char c);
int strToBinary(char* line);
uint64_t hashStr(char* str);
void removeCharacter(char* line, char c);
void mapPut(HASHMAP_ELEMENT_T map[], char* id, int value, long length);
HASHMAP_ELEMENT_T mapGet(HASHMAP_ELEMENT_T map[], char* id, long length);

#endif
