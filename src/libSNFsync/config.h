#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sn_instance.h"
#include "file.h"
#include "json.h"
#include "core.h"

#define KEY_VALUE_SEPERATOR "\":\""
#define KEY_SEPERATOR ","


#define SYS_ID "sys_id"
#define FILE_NAME "file_name"
#define TABLE "target_table"
#define RECORD "target_record"
#define TYPE "type"
#define SUBTYPE "subtype"
#define ACTION "actions"
#define FILE_TYPE "file_type"

#define DELETE_EXISTING_ATTACHMENTS "delete_existing_attachments"
#define REMOVE_LINES_FROM_BEGINNING "remove_lines_from_beginning"

struct SN_INSTANCE {
	char* host_name;
	char* username;
	char* password;
	struct SN_INSTANCE *next;
};

struct SN_ACTION {
	char* action;
	char* order;
	char* lines;
	struct SN_ACTION *next;
};

struct SN_SOURCE_RECORD {
	char* sys_id;
	char* file_name;
	char* table;
	char* record;
	char* type;
	char* subtype;
	char* file_type;
	struct SN_INSTANCE *instance;
	struct SN_SOURCE_RECORD *next;
	struct SN_ACTION *action;
};

typedef struct SN_SOURCE_RECORD SN_SOURCE_RECORD;
typedef struct SN_INSTANCE SN_INSTANCE;
typedef struct SN_ACTION SN_ACTION;

void push_all_files();

void generate_config_files(char *instance, char *username, char *password);
SN_SOURCE_RECORD* pull_sources_from_instance(SN_INSTANCE *instance);
void set_sys_sn_source_record(SN_SOURCE_RECORD **ds, char *sys);
void set_name_sn_source_record(SN_SOURCE_RECORD **ds, char *name);
void set_table_sn_source_record(SN_SOURCE_RECORD **sn_source, char *table);
void set_type_sn_source_record(SN_SOURCE_RECORD **sn_source, char *type);
void set_record_sn_source_record(SN_SOURCE_RECORD **sn_source, char *record);
void set_subtype_sn_source_record(SN_SOURCE_RECORD **sn_source, char *subtype);
void initialize_sn_source_record(SN_SOURCE_RECORD **sn_source);
void initialize_sn_instance(SN_INSTANCE **sn_instance);
SN_SOURCE_RECORD* parse_returned_sn_source_records(char *sources);

int is_sn_source_record_complete(SN_SOURCE_RECORD *to_test);

int seek_keys(char *str);
void free_source_list(SN_SOURCE_RECORD *head);
void free_instance_list(SN_INSTANCE *head);
void load_sn_instance_from_child_config_file(SN_INSTANCE **to_load, char *instance);
void load_sn_source_record_from_child_config(SN_SOURCE_RECORD **to_load, char *instance_dir_name);
int check_for_child_config_file(char *instance, char *prepend);
SN_INSTANCE* load_instances_from_files();
SN_SOURCE_RECORD* load_sources_from_sn_instance();
SN_SOURCE_RECORD* get_first_sn_source_record();
void load_action_from_string(SN_ACTION **to_load, char *action_string);

void call_send_to_instance();

#ifdef __cplusplus
}
#endif

#endif
