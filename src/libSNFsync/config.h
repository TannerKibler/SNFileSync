#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sn_instance.h"
#include "file.h"

#define KEY_VALUE_SEPERATOR "\":\""
#define KEY_SEPERATOR ","

struct SN_INSTANCE {
	char* host_name;
	struct SN_INSTANCE *next;
};

struct DATA_SOURCE {
	char* sys_id;
	char* name;
	struct DATA_SOURCE *next;
};

struct PUSH_CONFIG {
	char* file_name;
	char* file_to_prepend;
	char* file_to_append;
	char* file_name_filter;
	struct PUSH_CONFIG *next;
};

typedef struct DATA_SOURCE DATA_SOURCE;
typedef struct SN_INSTANCE SN_INSTANCE;
typedef struct PUSH_CONFIG PUSH_CONFIG;

void push_all_files();

void generate_config_files();
DATA_SOURCE* pull_sources_from_instance();
void set_sys_data_source(DATA_SOURCE **ds, char *sys);
void set_name_data_source(DATA_SOURCE **ds, char *name);
DATA_SOURCE* initialize_data_source();

PUSH_CONFIG* initialize_push_config();
void set_end_file_name(PUSH_CONFIG **push, char *name);
void set_prepend_file(PUSH_CONFIG **push, char *prep_file);
void set_append_file(PUSH_CONFIG **push, char *app_file);
void set_file_filter(PUSH_CONFIG **push, char *filter);
void parse_returned_data_sources(char *sources);

void seek_keys(char *str, char **retStr);

#ifdef __cplusplus
}
#endif

#endif
