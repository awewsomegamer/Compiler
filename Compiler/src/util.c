#include <util.h>
#include <global.h>

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
