#include <util.h>
#include <global.h>
#include <ctype.h>

bool startsWith(const char* str, const char* lookfor){
	if (strncmp(str, lookfor, strlen(lookfor)) == 0)
		return true;

	return false;
}

bool containsCharacter(char* line, char c){
	int contains_c = 0;

	for (int i = 0; i < strlen(line); i++) {
		if (*(line + i) == c){
			contains_c = 1;
			break;
		}
	}
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

void removeCharacter(char* line, char c){
	if (containsCharacter(line, c)){
		char* tmp = malloc(strlen(line));
		strcpy(tmp, line);
		memset(line, 0, strlen(tmp));

		int line_clean_i = 0;
		for (int i = 0; i < strlen(tmp); i++){
			if (*(tmp + i) != c){
				*(line + line_clean_i) = *(tmp + i);
				line_clean_i++;
			}
		}

		memset(tmp, 0, strlen(tmp));
		free(tmp);
	}
}
