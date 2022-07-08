#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <global.h>
#include <util.h>
#include <types.h>
#include <lexer.h>

bool _debug_msg = true;
int _line = 0;
int LINE_SIZE = 0;
char original_line[];

// char*** definitions = NULL;
// int definition_index = 0;

void fatal_err(char* message, ...){
	va_list args;
	va_start(args, message);
	
	printf("FATAL: ");
	vprintf(message, args);
	printf("\n");

	exit(1);
}

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
	char* in_file_name;
	bool in_file_set = false;

	FILE* out_file;
	char* out_file_name;
	bool out_file_set = false;

	// Interpret arguments
	for (int i = 1; i < argc; i++){
		if (startsWith(argv[i], "-o")){
			out_file_name = strdup(argv[i+1]);
			out_file = fopen(out_file_name, "w");
			out_file_set = true;
		}

		if (startsWith(argv[i], "-i")){
			if (i+2 < argc-1 && !startsWith(argv[i+2], "-")){
				in_file_name = "assemblery.asm";
				generate_assemblery(argv, i, argc);
			}else{
				in_file_name = strdup(argv[i+1]);
			}

			in_file = fopen(in_file_name, "r");
			in_file_set = true;
		}
	}

	assert(in_file_set == true);

	int file_length = 0;
	int current_line_length = 0;

	while (!feof(in_file)){
		if (fgetc(in_file) == '\n'){
			if (LINE_SIZE < current_line_length)
				LINE_SIZE = current_line_length;
			
			file_length++;
		}

		current_line_length++;
	}

	rewind(in_file);

	char line[LINE_SIZE];
	original_line[LINE_SIZE];

	HASHMAP_ELEMENT_T label_map[MAX_LABELS];

	if (_debug_msg) printf("------ FINDING LABELS ------\n");

	// Look for lines ending in ':' - labels.
	while (fgets(line, sizeof(line), in_file)){
		removeCharacter(line, '\n');
		removeCharacter(line, '\t');
		removeCharacter(line, ' ');

		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		if (endsWith(line, ':')){
			removeCharacter(line, ':');

			if (_debug_msg) printf("FOUND LABEL %s\n", line);

			mapPut(label_map, line, 0);
		}
	}
	
	rewind(in_file);
	
	if (_debug_msg) printf("------ ADDRESSING LABELS ------\n");

	// Define addresses of labels
	int address_count = 0; // Current address
	while (fgets(line, sizeof(line), in_file)){
		_line++;
		
		strcpy(original_line, line);

		removeCharacter(line, '\n');
		removeCharacter(line, '\t');
		removeCharacter(line, ' ');
		
		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		if (strlen(line) == 0) 
			continue;

		if (endsWith(line, ':')){
			removeCharacter(line, ':');

			if (_debug_msg) printf("FOUND LABEL %s, ADDRESSED AT %X\n", line, address_count);

			mapPut(label_map, line, address_count);
			continue;
		}

		strcpy(line, original_line);
		removeCharacter(line, '\n');
		removeCharacter(line, '\t');
		
		semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		TOKEN_T instruction = tokenize(line, label_map);
		int operation = instruction.operation & 0xFF;
		
		printf("[PRE] ADDRESS: %X LINE: %s\n", address_count, line);

		switch (INSTRUCTION_SET[operation].argc){
		case 0:
			address_count++;
			break;

		case 1: {
			uint8_t info_block = (instruction.operation >> 8) & 0xFF;
			int size = ((info_block >> 4)& 0b11) + 1;
			address_count += size + 2;
			break;
		}

		case 2: {
			uint8_t info_block = (instruction.operation >> 8) & 0xFF;
			int size = ((info_block >> 4)& 0b11) + ((info_block >> 6)& 0b11) + 2;
			address_count += size + 2;

			break;
		}

		case 3:
			if (operation == DEFINITION_BYTES)
				address_count += instruction.value1;
			else if (operation == DEFINITION_STRING)
				address_count += strlen(instruction.extra_bytes);
			
			break;
		}

		printf("[POST] ADDRESS: %X LINE: %s\n", address_count, line);
	}
	
	rewind(in_file);

	TOKEN_T instructions[file_length + 1];
	char* definitions[file_length + 1][LINE_SIZE];
	
	int instruction_index = 0;
	
	if (_debug_msg) printf("------ REAL ASSEMBLE ------\n");
	_line = 0;

	// Parse instructions and or definitions and put them into their respective arrays
	while (fgets(line, sizeof(line), in_file)){
		_line++;

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
		out_file_name = malloc(strlen(in_file_name)+4);
		sprintf(out_file_name, "%s.out", in_file_name);
		out_file = fopen(out_file_name, "w");
	}

	if (_debug_msg) printf("------ WRITING %s ------\n", out_file_name);

	// Write bytes
	for (int i = 0; i < instruction_index; i++){
		int operation = instructions[i].operation & 0xFF;

		switch (operation) {
		case DEFINITION_STRING:
			fputs(instructions[i].extra_bytes, out_file);
			if (_debug_msg) printf("DEFINED: %s\n", instructions[i].extra_bytes);

			break;

		case DEFINITION_BYTES:
			fwrite(instructions[i].extra_bytes, 1, instructions[i].value1, out_file);
				
			for (int j = 0; j < instructions[i].value1; j++) 
				if (_debug_msg) printf("DEFINED BYTE: %X\n", instructions[i].extra_bytes[j]);

			break;

		case INCLUDE:
			// Binary include
			if (instructions[i].value1 == 1)
				fwrite(instructions[i].extra_bytes, 1, instructions[i].value2, out_file);

			break;

		default:
			fwrite(&instructions[i].operation, (INSTRUCTION_SET[operation].argc == 0 ? 1 : 2), 1, out_file);

			uint8_t info_block = (instructions[i].operation >> 8) & 0xFF;
			int size1 = ((info_block >> 4)& 0b11) + 1;
			int size2 = ((info_block >> 6)& 0b11) + 1;

			if (INSTRUCTION_SET[operation].argc == 1 || INSTRUCTION_SET[operation].argc == 2)
				fwrite(&instructions[i].value1, size1, 1, out_file);

			if (INSTRUCTION_SET[operation].argc == 2)
				fwrite(&instructions[i].value2, size2, 1, out_file);

			if (_debug_msg) printf("WROTE: %04X ARGC: %d (%s)\n", instructions[i].operation, INSTRUCTION_SET[operation].argc, INSTRUCTION_SET[operation].key);

			break;
		}
	}

	fclose(out_file);
	fclose(in_file);

	return 0;
}
