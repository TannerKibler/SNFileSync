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

int find_index_of_next_occurence(char to_find, int start_index, char *search_string) {
	if (!to_find || search_string == NULL || !search_string[start_index] || search_string[start_index] == '\0') {
		// implement error library
		return -1;
	}
	printf("\n\nStarting on: %i", start_index);

	do {
		start_index++;
		if (search_string[start_index] == to_find) {
			printf("\nEnding on: %i\n", start_index);
			return start_index;
		}
	}
	while(search_string[start_index+1] != '\0');

	return -1;
}
