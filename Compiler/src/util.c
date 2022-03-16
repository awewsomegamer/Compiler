#include <util.h>

bool startsWith(const char* str, const char* lookfor){
	if (strncmp(str, lookfor, strlen(lookfor)) == 0)
		return true;

	return false;
}
