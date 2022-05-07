#include <lexer.h>
#include <global.h>
#include <util.h>

// Check if given name is a register, return it's value if so, otherwise return -1
int indexRegister(char* name){
	for (int i = 0; i < sizeof(REGISTER_T_NAMES)/sizeof(REGISTER_T_NAMES[0]); i++){
		if (strcmp(name, REGISTER_T_NAMES[i]) == 0){
			// Ending
			// X = FULL
			// L = /2
			// H = -1

			int index = i/3+1;

			// ABCD registers
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
		// Split line using the space character
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
		
		// Clean up sections
		for (; i < space_index; i++){
			strncpy(section_buffer, line+last_index,  space_indices[i]-last_index);
			last_index = space_indices[i];

			removeCharacter(section_buffer, ' ');
			removeCharacter(section_buffer, ',');

			strcpy(sections+i, section_buffer);
			memset(section_buffer, 0, LINE_SIZE);
		}

		// Clean up last line
		strncpy(section_buffer, line+last_index, strlen(line)-last_index);
		last_index = space_indices[i];

		removeCharacter(section_buffer, ' ');
		removeCharacter(section_buffer, ',');

		strcpy(sections+i, section_buffer);
		memset(section_buffer, 0, LINE_SIZE);

		// Check operation (sections[0])
		for (int i = 0; i < sizeof(OPERATION_T_NAMES)/sizeof(OPERATION_T_NAMES[0]); i++){
			if (strcmp(sections[0], OPERATION_T_NAMES[i]) == 0){
				result.operation = i;
				break;
			}
		}

		// Index values
		// 0 regular value (0xEEE and or label addresses)
		// 1 register value (AX = 0x10)
		// 2 register's value pointer ([AX] = [0x10])
		// 3 value of memory address ([0x10] or [LABEL])

		uint8_t indices = 0;

		char* section = malloc(sizeof(sections[i+1])); // Original section
		char* section_clean = malloc(sizeof(sections[i+1])); // Removed extraneous characters

		for (int i = 0; i < space_index; i++){
			// Clear sections
			memset(section, 0, sizeof(section));
			memset(section_clean, 0, sizeof(section));

			strcpy(section, sections[i+1]);
			strcpy(section_clean, sections[i+1]);

			removeCharacter(section_clean, '[');
			removeCharacter(section_clean, ']');
			
			// Index of this section
			uint8_t singular_index = 0;

			// Try to index register
			uint32_t value = indexRegister(section_clean);

			// If register is found, set the current index accordingly
			if (value != -1)
				singular_index = 1 + (*section == '[' ? 1 : 0);

			// If register is not found, conduct the following checks plain values
			if (value == -1){
				if (startsWith(section, "'")){
					// Char
					char* string = malloc(2);
					strncpy(string, section_clean, 2);

					value = (int)*(string+1);

					free(string);
				}else if (startsWith(section_clean, "0x")){
					// Hex
					value = strtol(section_clean, NULL, 16);
				}else if (startsWith(section_clean, "0b")){
					// Binary
					value = strToBinary(section_clean);
				}else if (isNumber(section_clean)){
					// Integer
					value = atoi(section_clean);
				}

				// If plain value is found, set the index accordingly (pointer or plain value)
				if (value != -1)
					singular_index = (*section == '[' ? 3 : 0);
			}

			// If value is still not found, check label map
			if (value == -1){
				HASHMAP_ELEMENT_T label = mapGet(label_map, section_clean);

				value = label.value;
				
				// If label is found, set the index accordingly (pointer or plain value)
				if (value != -1)
					singular_index = (*section == '[' ? 3 : 0);
			}

			// Set the values of the result token
			if (i == 0)
				result.value1 = value;
			else if (i == 1)
				result.value2 = value;

			// Add current index into final index byte
			indices |= singular_index << i * 4;
		}

		// When done, free sections
		free(section);
		free(section_clean);
		
		// Write the final indices into the higher byte of the result's operation
		result.operation |= indices << 8;
	}

	return result;
}
