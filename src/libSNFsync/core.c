#include "core.h"

void set_string_value(char* strSource, char** strDest) {
	printf("Setting strDest to : %s\n", strSource);
	if (strSource == NULL)
		return;

	*strDest = malloc(sizeof(char)*strlen(strSource) + 4);
	if (*strDest == NULL) {
		// error allocating
		// handle this
		return;
	}

	strcpy(*strDest, strSource);
}

char* substring(char *str, int stIndex, int enIndex) {
	char *retString = NULL;
	int loopIn = 0;
	
	if (enIndex < stIndex)
		return NULL;

	if ((enIndex - stIndex) > 1)
		retString = malloc((sizeof(char)*(enIndex - stIndex) + 8));
	else
		retString = malloc(32);
	while(stIndex <= enIndex && str[stIndex] != '\0') {
		retString[loopIn] = str[stIndex];
		stIndex++;
		loopIn++;
	}

	retString[loopIn] = '\0';

	return retString;
}

int backwards_find_index(char* str, char toFind) {
	if (str == NULL)
		return -1;

	int last = strlen(str)-1;
	for (last; last >= 0; last--) {
		if (str[last] == toFind)
			return last;
	}

	return -1;
}
