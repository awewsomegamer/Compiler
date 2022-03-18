#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <string.h>

bool startsWith(const char* str, const char* lookfor);
bool containsCharacter(char* line, char c);
void removeCharacter(char* line, char c);

#endif
