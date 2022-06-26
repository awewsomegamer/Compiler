#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <global.h>
#include <util.h>
#include <types.h>
#include <lexer.h>

char original_line[LINE_SIZE];
// char*** definitions = NULL;
// int definition_index = 0;

// Combine all given inputs into one output
void generate_assemblery(char* argv[], int i, int size){
	FILE* assemblery = fopen("assemblery.asm", "w");
	FILE* assemblees[size];
	int a_i = 0;
	int i_prev = i;

	// Read and open all files
	for (; i < size; i++)
		if (!startsWith(argv[i], "-"))
			assemblees[a_i++] = fopen(argv[i], "r");

	char line[LINE_SIZE];
	
	for (int j = 0; j < a_i; j++) {
		if (assemblees[j] != NULL){
			// If input is not null, then print it to assemblery (big main file) like this
			// ; path/of/assembly.file
			// file contents
			// \n
			// and then close it

			fprintf(assemblery, "; %s\n", argv[i_prev + j + 1]);

			while (fgets(line, sizeof(line), assemblees[j]))
				fprintf(assemblery, "%s", line);

			fprintf(assemblery, "\n");

			fclose(assemblees[j]);
		}
	}

	fclose(assemblery);
}

int main(int argc, char* argv[]){
	FILE* in_file;
	char* in_file_name = "assemblery.asm";
	bool in_file_set = false;

	FILE* out_file;
	bool out_file_set = false;

	// Interpret arguments
	for (int i = 1; i < argc; i++){
		if (startsWith(argv[i], "-o")){
			out_file = fopen(argv[i+1], "w");
			out_file_set = true;
		}

		if (startsWith(argv[i], "-i")){
			generate_assemblery(argv, i, argc);
			in_file = fopen(in_file_name, "r");
			in_file_set = true;
		}
	}

	assert(in_file_set == true);

	int file_length = 0;

	while (!feof(in_file))
		if (fgetc(in_file) == '\n')
			file_length++;

	rewind(in_file);

	char line[LINE_SIZE];

	HASHMAP_ELEMENT_T label_map[MAX_LABELS];
	int address_count = 0; // Current address

	// Look for lines ending in ':' - labels.
	while (fgets(line, sizeof(line), in_file)){
		// Clean line
		strcpy(original_line, line);

		// Find label
		removeCharacter(line, '\t');
		removeCharacter(line, '\n');
		removeCharacter(line, ' ');

		// Remove comments
		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;
	
		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		if (endsWith(line, ':')){
			printf("LABEL: %s ADDRESS: %X\n", line, address_count);

			removeCharacter(line, ':');
			mapPut(label_map, line, address_count);
		}

		// Calculate address
		
		strcpy(line, original_line);
		removeCharacter(line, '\t');
		removeCharacter(line, '\n');

		// Remove comments
		semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		TOKEN_T instruction = tokenize(line, label_map);

		printf("%d\n", OPERATION_T_ARGC[instruction.operation]);

		if (OPERATION_T_ARGC[instruction.operation] == 0)
			address_count++;

		if (OPERATION_T_ARGC[instruction.operation] == 1)
			address_count += 2 + (sizeInBytes(instruction.value1) + 1);

		if (OPERATION_T_ARGC[instruction.operation] == 2)
			address_count += 2 + (sizeInBytes(instruction.value1) + sizeInBytes(instruction.value2) + 2);

		if (instruction.operation == DEFINITION_BYTES || instruction.operation == DEFINITION_STRING)
			address_count += sizeof(instruction.extra_bytes);
	}
	
	rewind(in_file);

	TOKEN_T instructions[file_length + 1];
	char* definitions[file_length + 1][LINE_SIZE];
	
	int instruction_index = 0;
	
	printf("------ REAL COMPILE ------\n");

	// Parse instructions and or definitions and put them into their respective arrays
	while (fgets(line, sizeof(line), in_file)){
		strcpy(original_line, line);

		// Skip labels
		removeCharacter(line, '\n');
		removeCharacter(line, '\t');
		removeCharacter(line, ' ');

		// Remove comments
		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		if (endsWith(line, ':') || strlen(line) == 0)
			continue;


		strcpy(line, original_line);
		removeCharacter(line, '\n');
		removeCharacter(line, '\t');

		// Remove comments
		semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));		

		instructions[instruction_index++] = tokenize(line, label_map);
	}

	// If not out file is set, then an outfile will be generated with the in file's name + ".out"
	if (!out_file_set){
		char* name = malloc(strlen(in_file_name)+4);
		sprintf(name, "%s.out", in_file_name);
		out_file = fopen(name, "w");
		free(name);
	}

	// Write bytes
	for (int i = 0; i < instruction_index; i++){
		int operation = instructions[i].operation & 0xFF;

		if (operation > ENDFILE && operation < DEFINITION_STRING){
			fwrite(&instructions[i].operation, 2, 1, out_file);
			fwrite(&instructions[i].value1, sizeInBytes(instructions[i].value1) + 1, 1, out_file);
			fwrite(&instructions[i].value2, sizeInBytes(instructions[i].value2) + 1, 1, out_file);
		}
		
		if (operation == DEFINITION_STRING){
			fputs(instructions[i].extra_bytes, out_file);
			printf("DEFINED: %s\n", instructions[i].extra_bytes);
		}
		
		if (operation == DEFINITION_BYTES){
			fwrite(instructions[i].extra_bytes, 1, instructions[i].value1, out_file);
				
			for (int j = 0; j < instructions[i].value1; j++)
				printf("DEFINED BYTE: %X\n", instructions[i].extra_bytes[j]);
		}
	}

	fclose(out_file);
	fclose(in_file);

	return 0;
}
