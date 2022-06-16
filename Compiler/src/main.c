#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <global.h>
#include <util.h>
#include <types.h>
#include <lexer.h>

int main(int argc, char* argv[]){
	FILE* in_file;
	char* in_file_name = malloc(LINE_SIZE);
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
			in_file = fopen(argv[i+1], "r");
			strcpy(in_file_name, argv[i+1]);
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

	int address_count = 0; // Measured in 4 bytes (each instruction is a 32 bit int)

	HASHMAP_ELEMENT_T label_map[MAX_LABELS];

	// Look for lines ending in ':' - labels.
	while (fgets(line, sizeof(line), in_file)){
		char* original_line = strdup(line);

		// Clean line
		removeCharacter(line, '\t');
		removeCharacter(line, '\n');
		removeCharacter(line, '\r');
		removeCharacter(line, ' ');
		

		// Remove comments
		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		if (endsWith(line, ':')){
			printf("%s %X\n", line, address_count);

			removeCharacter(line, ':');
			mapPut(label_map, line, address_count);
		}

		// Only increment address if the line is a valid operation
		for (int i = 0; i < sizeof(OPERATION_T_NAMES)/sizeof(OPERATION_T_NAMES[0]); i++){
			if (startsWith(line, OPERATION_T_NAMES[i]) || startsWith(line, "db") || startsWith(line, "ds")){
				address_count += INSTRUCTION_SIZE_BYTES;
				break;
			}
		}
	}

	rewind(in_file);

	TOKEN_T instructions[file_length + 1];
	char* definitions[file_length + 1][LINE_SIZE];
	
	int instruction_index = 0;
	int definition_index = 0;

	// Parse instructions and or definitions and put them into their respective arrays
	while (fgets(line, sizeof(line), in_file)){
		char* original_line = strdup(line);

		removeCharacter(line, '\n');
		removeCharacter(line, '\t');

		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		instructions[instruction_index] = tokenize(line, label_map);
		
		// Check if bytes are being defined
		if (startsWith(line, "db ")){
			// db 0x0, 0xA, 0xD
			
			char* current_byte_string = malloc(4);
			int current_byte_string_idx = 0;
			int bytes_size = 1;
			
			for (int i = 0; i < strlen(line); i++) 
				if (*(line + i) == ',') 
					bytes_size++;

			printf("%d", bytes_size);

			uint8_t bytes[bytes_size];
			int byte_ptr = 0;

			for (int i = 2; i < strlen(line); i++){
				if (*(line + i) != ',')
					*(current_byte_string + current_byte_string_idx++) = *(line + i);
				
				if (*(line + i) == ',' || i >= strlen(line) - 1){
					bytes[byte_ptr] = strtol(current_byte_string, NULL, 16);
					printf("%s\n", current_byte_string);
					byte_ptr++;
					current_byte_string_idx = 0;
				}
			}
			
			strncpy(definitions[definition_index], bytes, bytes_size);

			TOKEN_T def_token;
			def_token.operation = DEFINITION_BYTES;
			def_token.value1 = definition_index;
			instructions[instruction_index] = def_token;

			definition_index++;

			free(current_byte_string);
		}

		// Check if a string is being defined
		if (startsWith(line, "ds ")){
			int org_ln_quote = 0;

			for (; org_ln_quote < strlen(original_line); org_ln_quote++)
				if (*(original_line + org_ln_quote) == '\"')
					break;

			strncpy(definitions[definition_index], original_line + org_ln_quote + 1, strlen(original_line) - (org_ln_quote * 1.75));

			TOKEN_T def_token;
			def_token.operation = DEFINITION_STRING;
			def_token.value1 = definition_index;
			instructions[instruction_index] = def_token;

			definition_index++;
		}

		instruction_index++;
	}

	// If not out file is set, then an outfile will be generated with the in file's name + ".out"
	if (!out_file_set){
		char* name = malloc(strlen(in_file_name)+4);
		sprintf(name, "%s.out", in_file_name);
		
		out_file = fopen(name, "w");
		free(name);
	}

	// Write bytes
	instruction_index = 0;
	for (; instruction_index < file_length + 1; instruction_index++){
			// printf("%d\n", instructions[instruction_index].operation & 0xFF);
		if (instructions[instruction_index].operation > ENDFILE){
			putw((int)instructions[instruction_index].operation, out_file);
			putw((int)instructions[instruction_index].value1, out_file);
			putw((int)instructions[instruction_index].value2, out_file);
		}
		
		if (instructions[instruction_index].operation == DEFINITION_STRING){
			fputs(definitions[instructions[instruction_index].value1], out_file);
			// fputc(0, out_file);
		}
		
		if (instructions[instruction_index].operation == DEFINITION_BYTES){
			// printf("%c\n", definitions[instructions[instruction_index].value1][0]);
			fputs(definitions[instructions[instruction_index].value1], out_file);
		}
	}

	return 0;
}
