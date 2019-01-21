#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CUSTOM_PATH_CAP
	#define MAX_PATH CUSTOM_PATH_CAP
#endif
#ifndef CUSTOM_PATH_CAP
	#define MAX_PATH 150
#endif

void set_string_value(char* strSource, char** strDest);
char* substring(char* str, int stIndex, int enIndex);
int backwards_find_index(char* str, char toFind);

#ifdef __cplusplus
}
#endif

#endif
