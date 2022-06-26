#ifndef GLOBAL_H
#define GLOBAL_H

#define LINE_SIZE 256
#define MAX_LABELS 256

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern int _line;
extern char original_line[LINE_SIZE];

void fatal_err(char* message, ...);

// extern char*** definitions;
// extern int definition_index;

#endif