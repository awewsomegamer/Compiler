#include <lexer.h>
#include <global.h>
#include <util.h>

// Check if given name is a register, return it's value if so, otherwise return -1
REGISTER_T indexRegister(char* name){
	for (int i = 0; i < sizeof(REGISTER_T_NAMES)/sizeof(REGISTER_T_NAMES[0]); i++){
		if (strcmp(name, REGISTER_T_NAMES[i]) == 0){
			// Ending
			// X = FULL
			// L = /2
			// H = -1

			// ABCD registers
			if (i < 12){
				int index = (i / 3) * 0x10;

				switch((int)*(name+1)){
				case 'x': return index;
				case 'l': return index/2;
				case 'h': return index-1;
				}
			}

			if (i >= 12)
				return (i - 7) * 0x10;

			break;
		}
	}

	return -1;
}

TOKEN_T tokenize(char* line, HASHMAP_ELEMENT_T label_map[]){
	TOKEN_T result = {-1,0,0};
	
	if (strlen(line) >= 1){
		// Split line using the space character
		int space_indices[LINE_SIZE];

		int space_index = 0;

		bool instruction_started = false;

		for (int i = 0; i < strlen(line); i++){
			if (*(line + i) != ' ') instruction_started = true;

			if (*(line + i) == ' ' && instruction_started){
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
			// printf("-- %s --\n", line);
			// printf("%s -> %s [%d, %d]\n\n", sections[0], OPERATION_T_NAMES[i], i, strcmp(sections[0], OPERATION_T_NAMES[i]));

			if (strcmp(sections[0], OPERATION_T_NAMES[i]) == 0){
				result.operation = i;
				break;
			}
		}

		if (result.operation == INCLUDE){
			// Include type
			result.value1 = endsWith(sections[0], "#") ? 1 : 0; // Include type binary
			result.value1 = endsWith(sections[0], "%") ? 2 : 0; // Include type code

			char* path = malloc(strlen(line));
			int path_i = 0;
			bool copy = false;

			// Parse path from quotation marks
			for (int i = 0; i < strlen(line); i++){
				if (*(line + i) == '"') copy = !copy;
				if (copy) *(path + (path_i++)) = *(line + i);
			}

			result.extra_bytes = strdup(path);

			free(path);

			return result;
		}

		uint8_t value_count = 0;

		// Index values (1 byte)
		// 0 regular value (0xEEE and or label addresses)
		// 1 register value (AX = 0x10)
		// 2 register's value pointer ([AX] = [0x10])
		// 3 value of memory address ([0x10] or [LABEL])

		uint8_t indices = 0;

		// Size values (1 byte)
		// 0 byte
		// 1 word
		// 2 dword

		uint8_t sizes = 0;

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
			
			// Check if section is size specifier
			bool is_size = false;
			for (int s = 0; s < SIZE_T_MAX; s++){
				if (strcmp(section_clean, SIZE_T_NAMES[s]) == 0){
					sizes |= s << i * 4;
					is_size = true;
					break;
				}
			}

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
			if (value_count == 0 && !is_size){
				result.value1 = value;
				value_count++;
			} else if (value_count == 1 && !is_size){
				result.value2 = value;
				value_count++;
			}

			// Add current index into final index byte
			indices |= singular_index << i * 4;
		}

		// When done, free sections
		free(section);
		free(section_clean);
		
		// Write information about operation
		result.operation |= indices << 8;
		result.operation |= sizes << 16;

		printf("OPERATION: %08X INDICES: %04X SIZES: %04X VALUE1: %08X VALUE2: %08X [LINE: %s]\n", result.operation, indices, sizes, result.value1, result.value2, line);
	}

	return result;
}
