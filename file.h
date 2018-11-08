#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINDOWS
	#include <windows.h>
#endif

#include <stdio.h>
#include "core.h"

int ensure_dir_exists(char* l1, char* l2);
int ensure_file_exists(char* l1, char* l2);
char* get_current_directory();
char* read_file_to_buffer(char* fileName);

#ifdef __cplusplus
}
#endif

#endif