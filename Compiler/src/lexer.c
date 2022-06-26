#include <lexer.h>
#include <global.h>
#include <util.h>
#include <string.h>

// Check if given name is a register, return it's value if so, otherwise return -1
int indexRegister(char* name){
	for (int i = 0; i < sizeof(REGISTER_T_NAMES)/sizeof(REGISTER_T_NAMES[0]); i++){
		if (strcmp(name, REGISTER_T_NAMES[i]) == 0){
			return i;
			break;
		}
	}

	return -1;
}

TOKEN_T tokenize(char* line, HASHMAP_ELEMENT_T label_map[]){
	TOKEN_T result = {-1,0,0};

	if (strlen(line) >= 1){
		// Split line using the space character
		int space_index = 0;

		bool instruction_started = true;
		char* sections[strlen(line) + 1];
		char* section_buffer = malloc(strlen(line));
		int sect_c_ptr = 0;
		
		// Terminate line with a space
		char* string_sterm = malloc(strlen(line) + 1);
		strcpy(string_sterm, line);
		string_sterm[strlen(line)] = ' ';

		for (int i = 0; i < strlen(string_sterm); i++) {
			char c = *(string_sterm + i);

			if (c != ' '){
				instruction_started = true;
			} else if (c == ' '){
				instruction_started = false;

				removeCharacter(section_buffer, ' ');
				removeCharacter(section_buffer, ',');

				sections[space_index] = strdup(section_buffer);

				for (int j = 0; j < strlen(line); j++)
					section_buffer[j] = '\0';

				sect_c_ptr = 0;
				space_index++;
			}

			if (instruction_started)
				section_buffer[sect_c_ptr++] = c;
		}

		free(section_buffer);
		free(string_sterm);

		// Check operation (sections[0])
		uint16_t operation = 0;
		for (int i = 0; i < sizeof(OPERATION_T_NAMES)/sizeof(OPERATION_T_NAMES[0]); i++){
			// printf("-- %s --\n", line);
			// printf("%s -> %s [%d, %d]\n\n", sections[0], OPERATION_T_NAMES[i], i, strcmp(sections[0], OPERATION_T_NAMES[i]));

			if (strcmp(sections[0], OPERATION_T_NAMES[i]) == 0){
				operation = i;
				
				break;
			}
		}
		
		if (operation == INCLUDE){
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

		if (operation == DEFINITION_BYTES){
			if (_debug_msg) printf("PARSING BYTE DEFINITION [%s]\n", line);

			// db 0x0, 0xA, 0xD
			char current_byte_string[4];
			int current_byte_string_idx = 0;
			int bytes_size = 1;
			

			for (int i = 0; i < strlen(line); i++) 
				if (*(line + i) == ',') 
					bytes_size++;

			result.extra_bytes = malloc(bytes_size);
			int byte_ptr = 0;

			for (int i = 2; i < strlen(line); i++){
				if (*(line + i) != ',')
					*(current_byte_string + current_byte_string_idx++) = *(line + i);
				
				if (*(line + i) == ',' || i >= strlen(line) - 1){
					result.extra_bytes[byte_ptr] = strtol(current_byte_string, NULL, 16);
					if (_debug_msg) printf("BYTE: %X\n", result.extra_bytes[byte_ptr]);
					byte_ptr++;
					current_byte_string_idx = 0;
				}
			}

			result.value1 = bytes_size;
			result.operation = DEFINITION_BYTES;
			
			return result;
		}

		if (operation == DEFINITION_STRING){
			if (_debug_msg) printf("PARSING STRING DEFINITION [%s]\n", line);

			int quote_indices[2];
			int quotes_found = 0;

			for (int i = 0; i < strlen(original_line) - 1; i++)
				if (*(original_line + i) != '\\' && *(original_line + i) == '"')
					quote_indices[quotes_found++] = i;
			
			char* string = malloc(quote_indices[1] - quote_indices[0]);
			strncpy(string, original_line + quote_indices[0] + 1, quote_indices[1] - quote_indices[0] - 1);

			if (_debug_msg) printf("STRING: %s\n", string);

			result.operation = DEFINITION_STRING;
			result.extra_bytes = strdup(string);

			free(string);

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

		// uint8_t sizes = 0;

		char section[LINE_SIZE]; // Original section
		char section_clean[LINE_SIZE]; // Removed extraneous characters

		int fixed_sizes[2];

		for (int i = 1; i < space_index; i++){
			// Clear sections
			memset(section, 0, sizeof(section));
			memset(section_clean, 0, sizeof(section));

			strcpy(section, sections[i]);
			strcpy(section_clean, sections[i]);

			removeCharacter(section_clean, '[');
			removeCharacter(section_clean, ']');
			
			// printf("[%d]: %s\n", i, sections[i]);

			// Check if section is size specifier
			// bool is_size = false;
			// for (int s = 0; s < SIZE_T_MAX; s++){
			// 	if (strcmp(section_clean, SIZE_T_NAMES[s]) == 0){
			// 		sizes |= s << i * 4;
			// 		is_size = true;
			// 		break;
			// 	}
			// }

			// Index of this section
			uint8_t singular_index = 0;

			// Try to index register
			uint32_t value = indexRegister(section_clean);

			// Fixed size of value in output (bytes)
			int fixed_size = -1;

			// If register is found, set the current index accordingly
			if (value != -1)
				singular_index = (*section == '[' ? 2 : 1);

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

				// printf("LNAME %s LVAL %d\n", label.id, label.value);

				value = label.value;
				
				fixed_size = 4;

				// If label is found, set the index accordingly (pointer or plain value)
				if (value != -1)
					singular_index = (*section == '[' ? 3 : 0);
			}

			// Undefined reference
			if (value == -1)
				fatal_err("Undefined reference to <%s> on line <%d>", sections[i], _line);
			

			// Set the values of the result token
			if (value_count == 0){
				result.value1 = value;
				fixed_sizes[0] = fixed_size;
				value_count++;
			} else if (value_count == 1){
				result.value2 = value;
				fixed_sizes[1] = fixed_size;
				value_count++;
			}

			// printf("I:%d INDEX:%d %s\n", i, singular_index, sections[i]);

			// Add current index into final index byte
			indices |= singular_index << (i * 2);
		}
		
		// Write information about operation
		operation |= indices << 6; // Indices
		operation |= ((fixed_sizes[0] == -1) ? (sizeInBytes(result.value1) << 12) : ((fixed_sizes[0] - 1) << 12)); // Arg 1 size
		operation |= ((fixed_sizes[1] == -1) ? (sizeInBytes(result.value2) << 14) : ((fixed_sizes[1] - 1) << 14)); // Arg 2 size
		result.operation = operation;

		// printf("SIZE IN BYTES V1: %d V2: %d\n", sizeInBytes(result.value1), sizeInBytes(result.value2));

		if (_debug_msg){
			printf("OP_BITS: ");
			printBinary(operation);
			printf(" OPERATION: %04X INDICES: %04X VALUE1: %08X VALUE2: %08X [LINE: %s]", result.operation & 0xFF, indices, result.value1, result.value2, line);
			printf("\n");
		}
	}
	
	return result;
}
