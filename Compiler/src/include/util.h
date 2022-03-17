#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <string.h>

bool startsWith(const char* str, const char* lookfor);
void removeCharacter(char* line, char c);

#endif
