#include <util.h>
#include <global.h>

bool startsWith(const char* str, const char* lookfor){
	if (strncmp(str, lookfor, strlen(lookfor)) == 0)
		return true;

	return false;
}

void removeCharacter(char* line, char c){
	int contains_c = 0;

	for (int i = 0; i < strlen(line); i++)
		if (*(line + i) == c)
			contains_c = 1;

	if (contains_c){
		char* tmp = malloc(strlen(line));
		memcpy(tmp, line, strlen(line));

		int line_clean_i = 0;
		for (int i = 0; i < LINE_SIZE; i++){
			if (*(tmp + i) != c){
				*(line + line_clean_i) = *(tmp + i);
				line_clean_i++;
			}
		}

		free(tmp);
	}
}
