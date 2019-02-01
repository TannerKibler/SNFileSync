#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>
#include "file.h"

#ifdef CUSTOM_PATH_CAP
	#define MAX_PATH CUSTOM_PATH_CAP
#endif
#ifndef CUSTOM_PATH_CAP
	#define MAX_PATH 150
#endif

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#define INOTIFY_EVENT_SIZE (sizeof(struct inotify_event))
#define INOTIFY_BUFFER_LENGTH (1024 * (INOTIFY_EVENT_SIZE + 16))

typedef struct SN_SOURCE_RECORD SN_SOURCE_RECORD;

void set_string_value(char* strSource, char** strDest);
char* substring(char* str, int stIndex, int enIndex);
int backwards_find_index(char* str, char toFind);
int find_index_of_next_occurence(char next_char, int start_index, char *search_string);
void start_watching_landing_directory_for_instance();
void* poll_changes_to_landing_directory(void *thread_data);
void* loop_for_file_system_watch(void *thread_data);
int watch_for_file_system_changes();
int create_daemon();
void process_new_files();
int backwards_find_index_from_index(char *str, char toFind, int start);
void handle_file_by_source_record_rules(char *file_name, SN_SOURCE_RECORD *sn_source_record);
int check_file_name_for_match(char *file_name, SN_SOURCE_RECORD *sn_source_record);

#ifdef __cplusplus
}
#endif

#endif
