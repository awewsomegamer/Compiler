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

// void math_get_value_string(char* string, int offset, char* ret){
// 	memset(ret, 0, strlen(string));

// 	int x = 0;

// 	while (true){
// 		char c_c = *(string + offset + x);
		
// 		if (c_c == '+' || c_c == '-' || c_c == '*' || c_c == '/' || c_c == ' ' || c_c == '\t')
// 			break;

// 		ret[x] = c_c;

// 		x++;
// 	}

// 	printf("%s %d\n", string, offset);
// }

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
		for (int i = 0; i < INSTRUCTION_MAX; i++){
			// printf("-- %s --\n", line);
			// printf("%s -> %s [%d, %d]\n\n", sections[0], OPERATION_T_NAMES[i], i, strcmp(sections[0], OPERATION_T_NAMES[i]));

			if (strcmp(sections[0], INSTRUCTION_SET[i].key) == 0){
				operation = i;
				
				break;
			}
		}
		
		if (strcmp(sections[0], "inc#") == 0 || strcmp(sections[0], "inc%") == 0){
			// Include type
			result.operation = INCLUDE;
			result.value1 = endsWith(sections[0], '#') ? 1 : 0; // Include type binary

			if (result.value1 == 0)
				result.value1 = endsWith(sections[0], '%') ? 2 : 0; // Include type code

			char* path = malloc(strlen(line));
			int path_i = 0;
			bool copy = false;

			// Parse path from quotation marks
			for (int i = 0; i < strlen(line); i++){
				if (*(line + i) == '"') copy = !copy;
				else if (copy) *(path + (path_i++)) = *(line + i);
			}

			// Read in binary
			if (result.value1 == 1){
				printf("INCLUDING BINARY: %s\n", path);

				FILE* binary;
				binary = fopen(path, "r");

				fseek(binary, 0, SEEK_END);
				int size = ftell(binary);
				fseek(binary, 0, SEEK_SET);

				result.value2 = size;
				result.extra_bytes = malloc(size);

				fread(result.extra_bytes, 1, size, binary);
			}
			
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

			int quote_indices[2] = {0, 0};
			int quotes_found = 0;

			for (int i = 0; i < strlen(original_line) - 1; i++)
				if (*(original_line + i) != '\\' && *(original_line + i + 1) == '"')
					quote_indices[quotes_found++] = i;
			
			if (_debug_msg) printf("LINE: %s I1:%d I2:%d\n", original_line, quote_indices[0], quote_indices[1]);

			char string[quote_indices[1] - quote_indices[0]];
			memset(string, 0, quote_indices[1] - quote_indices[0]);
			strncpy(string, original_line + quote_indices[0] + 2, quote_indices[1] - quote_indices[0] - 1);

			if (_debug_msg) printf("STRING: %s\n", string);

			result.operation = DEFINITION_STRING;
			result.extra_bytes = strdup(string);

			// free(string);

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
			
			// If value is still not found, check if it contains expression keywords if not then it is most likely a label, check label map
			if (value == -1 && !containsCharacter(section_clean, '+') && !containsCharacter(section_clean, '-') && !containsCharacter(section_clean, '*') && !containsCharacter(section_clean, '/')){
				HASHMAP_ELEMENT_T label = mapGet(label_map, section_clean);

				// printf("LNAME %s LVAL %d\n", label.id, label.value);

				value = label.value;
				
				fixed_size = 4;

				// If label is found, set the index accordingly (pointer or plain value)
				if (value != -1)
					singular_index = (*section == '[' ? 3 : 0);
			}

			// Check for expression
			if (value == -1){
				if (_debug_msg) printf("DOING EXPRESSION\n");

				// Tokenize string into tokens
				EXPR_TOKEN_T tokens[strlen(section_clean)];
				int expr_token_ptr = 0;
				
				for (int j = 0; j < strlen(section_clean); j++){
					char c = *(section_clean + j);

					switch (c){
					case '+':
						printf("ADD FOUND ");
						tokens[expr_token_ptr++].type = M_ADD;
						break;
					case '-':
						printf("SUB FOUND ");
						tokens[expr_token_ptr++].type = M_SUB;
						break;
					case '*':
						printf("MUL FOUND ");
						tokens[expr_token_ptr++].type = M_MUL;
						break;
					case '/':
						printf("DIV FOUND ");
						tokens[expr_token_ptr++].type = M_DIV;
						break;

					default:
						char name[strlen(section_clean)];
						memset(name, 0, strlen(section_clean));

						int x = 0;

						while (true){
							char c_c = *(section_clean + j + x);
							
							if (c_c == '+' || c_c == '-' || c_c == '*' || c_c == '/' || c_c == ' ' || c_c == '\t' || x == strlen(section_clean) - 1)
								break;

							name[x++] = c_c;
						}
						
						printf("FOUND LITERAL %s ", name);

						tokens[expr_token_ptr].type = M_INT;

						if (isNumber(name)){
							// Integer
							if (startsWith(name, "0x")){
								// Hex string
								tokens[expr_token_ptr++].value = strtol(name, NULL, 16);
							} else if (startsWith(name, "0b")) {
								// Binary
								tokens[expr_token_ptr++].value = strtol(name, NULL, 2);
							} else {
								tokens[expr_token_ptr++].value = strtol(name, NULL, 10);
							}
						} else {
							// Label
							tokens[expr_token_ptr++].value = mapGet(label_map, name).value;
						}
					}
				}

				printf("\n");

				// Evaluate tokens
				// base op value op value
				value = tokens[0].value;
				for (int j = 1; j < expr_token_ptr; j++){
					if (tokens[j].type < M_INT){
						// Math op found, preform operation on both sides and add to value
						int rhs = tokens[j + 1].value;
						printf("PRE(J:%d): VALUE: %d RHS: %d OPERATION: %d\n", j, value, rhs, tokens[j].type);
						
						switch (tokens[j].type){
						case M_ADD:
							value += rhs;
							break;
						case M_SUB:
							value -= rhs;
							break;
						case M_MUL:
							value *= rhs;
							break;
						case M_DIV:
							value /= rhs;
							break;
						}

						printf("POST: VALUE: %d RHS: %d OPERATION: %d\n", value, rhs, tokens[j].type);
					}
				}

				if (_debug_msg) printf("EXPRESSION: %s EVALUATED TO: %d\n", section_clean, value);
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
