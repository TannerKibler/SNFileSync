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

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

void set_string_value(char* strSource, char** strDest);
char* substring(char* str, int stIndex, int enIndex);
int backwards_find_index(char* str, char toFind);
int find_index_of_next_occurence(char next_char, int start_index, char *search_string);

#ifdef __cplusplus
}
#endif

#endif
