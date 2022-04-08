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

TOKEN_T tokenize(char* line, HASHMAP_ELEMENT_T label_map[]){
	TOKEN_T result = {-1,0,0};

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

		for (int i = 0; i < sizeof(OPERATION_T_NAMES)/sizeof(OPERATION_T_NAMES[0]); i++){
			if (strcmp(sections[0], OPERATION_T_NAMES[i]) == 0){
				result.operation = i;
				break;
			}
		}

		// Index values
		// 0 = none, 1 = first value, 2 = second value, 3 = both values, 4 = first value label, 5 = second value label, 6 = both values are labels
		uint8_t indices = 0;
		uint8_t register_count = 0;

		for (int i = 0; i < space_index; i++){
			uint32_t value = indexRegister(sections[i+1]);
			if (value != -1) register_count++;

			if (value == -1){
				if (indices == 0) indices = i;
				else indices++;

				// Check if section is reffering plain value (int, char, etc...)
				
				if (startsWith(sections[i+1], "'")){
					// Char
					char* string = malloc(2);
					strncpy(string, sections[i+1], 2);

					value = (int)*(string+1);

					free(string);
				}else if (startsWith(sections[i+1], "0x")){
					// Hex
					value = strtol(sections[i+1], NULL, 16);
				}else if (startsWith(sections[i+1], "0b")){
					// Binary
					value = strToBinary(sections[i+1]);
				}else if (isNumber(sections[i+1])){
					// Integer
					value = atoi(sections[i+1]);
				}
			}
			// printf("%d < VALUE <%s\n", value, sections[i+1]);

			// Not plain value, check labels
			// ISSUE: Labels that are not valid, are included in this if statement
			// and written to the result.
			if (value == -1){
				if (indices == 0) indices = 4;
				else indices++;

				HASHMAP_ELEMENT_T label = mapGet(label_map, sections[i+1]);

				value = label.value;
			}
			// Otherwise get address of label if section does not start with '[' and end with ']'
			// When label starts with '[' and ends with ']' return value of label

			if (i == 0)
				result.value1 = value;
			else if (i == 1)
				result.value2 = value;

			// printf("%s\n", sections[i+1]);
		}
		
		if (register_count == 2) indices = 3;
		else indices = register_count;

		result.operation |= indices << 8;
	}

	return result;
}
