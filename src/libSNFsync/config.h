#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sn_instance.h"
#include "file.h"
#include "json.h"

#define KEY_VALUE_SEPERATOR "\":\""
#define KEY_SEPERATOR ","

struct SN_INSTANCE {
	char* host_name;
	struct SN_INSTANCE *next;
};

struct SN_SOURCE_RECORD {
	char* sys_id;
	char* file_name;
	char* table;
	char* record;
	char* type;
	char* subtype;
	struct SN_SOURCE_RECORD *next;
};

struct PUSH_CONFIG {
	char* file_name;
	char* file_to_prepend;
	char* file_to_append;
	char* file_name_filter;
	struct PUSH_CONFIG *next;
};

typedef struct SN_SOURCE_RECORD SN_SOURCE_RECORD;
typedef struct SN_INSTANCE SN_INSTANCE;
typedef struct PUSH_CONFIG PUSH_CONFIG;

void push_all_files();

void generate_config_files();
SN_SOURCE_RECORD* pull_sources_from_instance();
void set_sys_sn_source_record(SN_SOURCE_RECORD **ds, char *sys);
void set_name_sn_source_record(SN_SOURCE_RECORD **ds, char *name);
void set_table_sn_source_record(SN_SOURCE_RECORD **sn_source, char *table);
void set_type_sn_source_record(SN_SOURCE_RECORD **sn_source, char *type);
void set_record_sn_source_record(SN_SOURCE_RECORD **sn_source, char *record);
void set_subtype_sn_source_record(SN_SOURCE_RECORD **sn_source, char *subtype);
SN_SOURCE_RECORD* initialize_sn_source_record();

PUSH_CONFIG* initialize_push_config();
void set_end_file_name(PUSH_CONFIG **push, char *name);
void set_prepend_file(PUSH_CONFIG **push, char *prep_file);
void set_append_file(PUSH_CONFIG **push, char *app_file);
void set_file_filter(PUSH_CONFIG **push, char *filter);
void parse_returned_sn_source_records(char *sources);

int seek_keys(char *str);

#ifdef __cplusplus
}
#endif

#endif
