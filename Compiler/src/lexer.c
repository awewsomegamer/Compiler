#include <lexer.h>
#include <global.h>
#include <util.h>

TOKEN_T tokenize(char* line){
	TOKEN_T result = {ENDFILE,0,0};

	if (strlen(line) > 1){
		int space_indices[LINE_SIZE];

//		size_t space_count = 0;

		int space_index = 0;
		for (int i = 0; i < strlen(line); i++){
			if (*(line + i) == ' '){
				space_indices[space_index] = i;
				space_index++;
			}
		}

//		printf("line :: %s", line);
//		printf("        ");
//		for (int i = 0; i < strlen(line); i++)
//			printf("%d", i);
//
//		printf("\nspace indices ::");
//
//		for (int i = 0; i < space_index; i++)
//			printf("%d ", space_indices[i]);
//
//		printf("\n");

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

		for (i = 0; i < space_index+1; i++)
			printf("%s : ", sections[i]);
		printf("\n");

//		free(sections);
//		free(section_buffer);

//		printf("%d :: %s\n", space_count,line);
//
//		char* sections[space_count+1];
//		char* section = malloc(LINE_SIZE);
//		int sect_ptr = 0;
//		int section_c_ptr = 0;
//
//		for (int i = 0; i < strlen(line); i++){
//			*(section + section_c_ptr) = *(line + i);
//			section_c_ptr++;
//
//			if (*(line + i) == ' '){
//				sections[sect_ptr] = section;
//				section_c_ptr = 0;
//				sect_ptr++;
//				memset(section, 0, strlen(section));
//			}
//		}
//
//		for (size_t i = 0; i < space_count; i++)
//			printf("%s : %d : Actual loook : %s\n",sections[i], i, line);
//		printf("--------------------\n");

//		free(section);
	}

	return result;
}

