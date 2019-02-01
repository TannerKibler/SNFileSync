#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINDOWS
	#include <windows.h>
#endif
#ifndef WINDOWS
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <errno.h>
#endif

#include <stdio.h>
#include "core.h"
#include "config.h"

typedef struct SN_SOURCE_RECORD SN_SOURCE_RECORD;
typedef struct SN_INSTANCE SN_INSTANCE;

int ensure_dir_exists(char *buffer, char* l1, char* l2);
int ensure_file_exists(char *buffer, char* l1, char* l2);
char* get_current_directory(char *buffer);
char* read_file_to_buffer(char* fileName);
void generate_config_file_from_source_record(SN_SOURCE_RECORD *to_store, char *path, char *file_name);
void generate_file_for_instance_config(SN_INSTANCE *instance, char *path, char *file_name);
int validate_directory(char *directory);
char** read_files_in_landing_directory();
char** read_files_in_directory(char *directory);

#ifdef __cplusplus
}
#endif

#endif
