#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include <assert.h>

#define LINE_SIZE 256

int main(int argc, char* argv[]){
	FILE* in_file;
	FILE* out_file;

	for (int i = 1; i < argc; i++){
		if (startsWith(argv[i], "-o") == true)
			out_file = fopen(argv[i+1], "w");

		if (startsWith(argv[i], "-i") == true)
			in_file = fopen(argv[i+1], "r");
	}

	assert(in_file != NULL);

	char line[LINE_SIZE];
	char* line_clean = malloc(LINE_SIZE);

	while (fgets(line, sizeof(line), in_file)){
		memset(line_clean, 0, LINE_SIZE);

		int line_clean_i = 0;
		for (int i = 0; i < LINE_SIZE; i++){
			if (line[i] != '\t'){
				*(line_clean+line_clean_i) = line[i];
				line_clean_i++;
			}
		}

		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line_clean+semcol_index) == ';')
				break;

		memset((line_clean + semcol_index), 0, (LINE_SIZE-semcol_index));

		// lex clean line
		printf("%s",line_clean);
	}

	free(line_clean);

	// compiler
	// output

	return 0;
}
