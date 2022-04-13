#include <stdlib.h>
#include <stdio.h>
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

	// LIST_T* labels_list_head = (LIST_T*)malloc(sizeof(LIST_T));

	while (fgets(line, sizeof(line), in_file)){
		removeCharacter(line, '\t');
		removeCharacter(line, '\n');
		removeCharacter(line, '\r');
		removeCharacter(line, ' ');

		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		if (endsWith(line, ':')){
			removeCharacter(line, ':');
			mapPut(label_map, line, address_count);
		}

		for (int i = 0; i < sizeof(OPERATION_T_NAMES)/sizeof(OPERATION_T_NAMES[0]); i++){
			if (startsWith(line, OPERATION_T_NAMES[i])){
				address_count += INSTRUCTION_SIZE_BYTES;
				break;
			}
		}
	}

	// current = labels_list_head;

	// while (current->next != NULL){
		// printf("%d : %s\n",current->lvalue.address, current->lvalue.name);

		// current = current->next;
	// }

	// Read file, detect labels, note down their address, name, and value into an array
	// Lexer uses data of labels to fill in the references of labels in code


	rewind(in_file);


	TOKEN_T instructions[file_length + 1];
	char* definitions[file_length + 1];

	int instruction_index = 0;
	int definition_index = 0;

	while (fgets(line, sizeof(line), in_file)){
		char* original_line = malloc(sizeof(line));
		strcpy(original_line, line);
		
		removeCharacter(line, '\n');
		removeCharacter(line, '\t');

		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		instructions[instruction_index] = tokenize(line, label_map);
		
		// printf("OPERATION: %u\nINDEX: %u\nV1: %u\nV2: %u\n\n", instructions[instruction_index].operation & 0xFF, instructions[instruction_index].operation & 0xFF00, instructions[instruction_index].value1, instructions[instruction_index].value2);
		
		if (startsWith(line, "db")){
			// parse bytes
			// 0x1, 0x2, 0x3
		}

		if (startsWith(line, "ds")){
			int org_ln_quote = 0;

			for (; org_ln_quote < strlen(original_line); org_ln_quote++)
				if (*(original_line + org_ln_quote) == '\"')
					break;

			strncpy(definitions[definition_index], original_line + org_ln_quote + 1, strlen(original_line));
			memset(definitions[definition_index] + strlen(definitions[definition_index]) - 2, 0, 1);

			TOKEN_T def_token;
			def_token.operation = DEFINITION;
			def_token.value1 = definition_index;
			instructions[instruction_index] = def_token;

			definition_index++;
		}

		instruction_index++;
	}

	// compiler
	// output

	if (!out_file_set){
		char* name = malloc(strlen(in_file_name)+4);
		sprintf(name, "%s.out", in_file_name);
		
		out_file = fopen(name, "w");
		free(name);
	}


	instruction_index = 0;
	for (; instruction_index < file_length; instruction_index++){
		// printf("%d : %d : %d\n",instructions[instruction_index].operation  >> 8 & 0xFF, instructions[instruction_index].value1, instructions[instruction_index].value2);

	// 	uint16_t operation = current->value.operation;
	// 	uint16_t v1 = current->value.value1;
	// 	uint16_t v2 = current->value.value2;

		if (instructions[instruction_index].operation > -1){
			putw((int)instructions[instruction_index].operation, out_file);
			putw((int)instructions[instruction_index].value1, out_file);
			putw((int)instructions[instruction_index].value2, out_file);
		}else if (instructions[instruction_index].operation == DEFINITION){
			for (int i = 0; i < strlen(definitions[instructions[instruction_index].value1]) + 1; i++)
				fwrite(((definitions[instructions[instruction_index].value1] + i)), 1, sizeof(char), out_file);
		}

	// 	current = current->next;
	}

	return 0;
}
