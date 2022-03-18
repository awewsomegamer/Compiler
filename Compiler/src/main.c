#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <global.h>
#include <util.h>
#include <types.h>
#include <lexer.h>

int main(int argc, char* argv[]){
	FILE* in_file;
	FILE* out_file;

	char* in_file_name = malloc(LINE_SIZE);

	for (int i = 1; i < argc; i++){
		if (startsWith(argv[i], "-o"))
			out_file = fopen(argv[i+1], "w");

		if (startsWith(argv[i], "-i")){
			in_file = fopen(argv[i+1], "r");
			strcpy(in_file_name, argv[i+1]);
		}
	}

	assert(in_file != NULL);

	char line[LINE_SIZE];

	LIST_T* head = (LIST_T*)malloc(sizeof(LIST_T));
	LIST_T* current = head;

//	LIST_T* labels_head = (LIST_T*)malloc(sizeof(LIST_T));
//	LIST_T* labels_current = labels_head;
//
//	LIST_T* labels_value_head = (LIST_T*)malloc(sizeof(LIST_T));
//	LIST_T* labels_value_current = labels_value_head;
//
//	int address_count = 0;

	while (fgets(line, sizeof(line), in_file)){
		removeCharacter(line, '\t');

		int semcol_index = 0;
		for (; semcol_index < LINE_SIZE; semcol_index++)
			if (*(line+semcol_index) == ';')
				break;

		memset((line + semcol_index), 0, (LINE_SIZE-semcol_index));

		current->value = tokenize(line);

		LIST_T* next = (LIST_T*)malloc(sizeof(LIST_T));
		current->next = next;

		current = next;

//		address_count += 3;
	}

	// compiler
	// output

	if (out_file == NULL){
		char* name = malloc(strlen(in_file_name)+4);
		sprintf(name, "%s.out", in_file_name);

		out_file = fopen(name, "w");
		free(name);
	}

	current = head;
	while (current->next != NULL){
		printf("%d : %d : %d\n",current->value.operation, current->value.value1, current->value.value2);

		putw((int)current->value.operation, out_file);
		putw((int)current->value.value1, out_file);
		putw((int)current->value.value2, out_file);

		current = current->next;
	}

	return 0;
}
