#include <lexer.h>
#include <global.h>
#include <util.h>

REGISTER_T indexRegister(char* name){
	for (int i = 0; i < sizeof(REGISTER_T_NAMES)/sizeof(REGISTER_T_NAMES[0]); i++){
		if (strcmp(name, REGISTER_T_NAMES[i]) == 0){
			// Ending
			// X = FULL
			// L = /2
			// H = -1

			int index = i/3+1;

			// ABCD register
			if (index < 5){
				index *= 0x10;

				switch((int)*(name+1)){
				case 'x': return index;
				case 'l': return index/2;
				case 'h': return index-1;
				}
			}

			if (index >= 5)
				return index*0x10;

			break;
		}
	}

	return -1;
}

TOKEN_T tokenize(char* line){
	TOKEN_T result = {NOP,NOP,NOP};

	if (strlen(line) > 1){
		int space_indices[LINE_SIZE];

		int space_index = 0;
		for (int i = 0; i < strlen(line); i++){
			if (*(line + i) == ' '){
				space_indices[space_index] = i;
				space_index++;
			}
		}

		char* sections[space_index+1][strlen(line)];
		char* section_buffer = malloc(LINE_SIZE);

		int last_index = 0;
		int i = 0;
		for (; i < space_index; i++){
			memcpy(section_buffer, line+last_index,  space_indices[i]-last_index);
			last_index = space_indices[i];

			removeCharacter(section_buffer, ' ');
			removeCharacter(section_buffer, ',');

			strcpy(sections+i, section_buffer);
			memset(section_buffer, 0, LINE_SIZE);
		}

		memcpy(section_buffer, line+last_index, strlen(line)-last_index);
		last_index = space_indices[i];

		removeCharacter(section_buffer, ' ');
		removeCharacter(section_buffer, ',');

		strcpy(sections+i, section_buffer);
		memset(section_buffer, 0, LINE_SIZE);

		// Operations and registers can only be lowercase

		for (int i = 0; i< sizeof(OPERATION_T_NAMES)/sizeof(OPERATION_T_NAMES[0]); i++){
			if (strcmp(sections[0], OPERATION_T_NAMES[i]) == 0){
				result.operation = i;
				break;
			}
		}

		// Index values
		// 0 = none, 1 = first value, 2 = second value, 3 = both values
		int register_indices = 0;

		for (int i = 0; i < space_index; i++){
			int value = indexRegister(sections[i+1]);

			if (value == -1){
				if (register_indices == 0) register_indices = i+1;
				else register_indices++;

				// Check if section is reffering plain value (int, char, etc...)

				if (*(sections[i+1]) == '\''){
					// Char
					value = *(sections[i]+1);
				}else if (startsWith(sections[i+1], "0x")){
					// Hex
					value = strtol(sections[i+1], NULL, 16);
				}else if (startsWith(sections[i+1], "0b")){
					// Binary
					value = strToBinary(sections[i+1]);
				}else{
					// Integer
					value = atoi(sections[i+1]);
				}
			}

			if (value == -1){
					
			}
			// Otherwise get address of label if section does not start with '[' and end with ']'
			// When label starts with '[' and ends with ']' return value of label

			if (i == 0)
				result.value1 = value;
			else if (i == 1)
				result.value2 = value;
		}

		result.operation |= register_indices << 8;
	}

	return result;
}
