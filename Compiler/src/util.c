#include <util.h>
#include <global.h>
#include <ctype.h>
#include <types.h>
#include <stdio.h>
#include <string.h>

bool startsWith(const char* str, const char* lookfor){
	if (strncmp(str, lookfor, strlen(lookfor)) == 0)
		return true;

	return false;
}

bool containsCharacter(char* line, char c){
	for (int i = 0; i < strlen(line); i++)
		if (*(line + i) == c)
			return true;

	return false;
}

bool endsWith(char* line, char c){
	if (strlen(line) >= 1 && *(line + strlen(line) - 1) == c)
		return true;

	return false;
}

bool isNumber(char* line){
	for (int i = 0; i < strlen(line); i++)
		if (!isdigit(*(line + i)))
			return false;

	return true;
}

// 0b included
int strToBinary(char* line){
	int result = 0;

	int multiplier = 1;
	for (size_t i = strlen(line)-1; i >= 2; i--){
		if (i <= 0) break;

		if (*(line+i) == '1')
			result += multiplier;

		multiplier *= 2;
	}

	return result/2;
}

// Returns the number of bytes the number uses - 1
int sizeInBytes(int v){
	int i = 0;

	while (v != 0){
		v >>= 8;
		if (v != 0) i++;
	}

	return i;
}

uint64_t hashStr(char* str){
	uint64_t hash = 559669;

	for (int i = 0; i < strlen(str); i++){
		hash ^= *(str + i);
		hash *= 1099511628211;
	}

	return hash;
}

void removeCharacter(char* line, char c){
	if (containsCharacter(line, c)){
		char* tmp = strdup(line);
		strcpy(tmp, line);
		memset(line, 0, strlen(tmp));

		int line_clean_i = 0;
		for (int i = 0; i < strlen(tmp); i++){
			if (*(tmp + i) != c){
				*(line + line_clean_i) = *(tmp + i);
				line_clean_i++;
			}
		}
	}
}

void mapPut(HASHMAP_ELEMENT_T map[], char* id, int value){
	HASHMAP_ELEMENT_T element;
	element.id = id;
	element.value = value;

	map[hashStr(id) % MAX_LABELS] =  element;
}

HASHMAP_ELEMENT_T mapGet(HASHMAP_ELEMENT_T map[], char* id){
	return map[hashStr(id) % MAX_LABELS];
}