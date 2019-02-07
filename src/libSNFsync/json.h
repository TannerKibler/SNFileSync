#ifndef JSON_H
#define JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include "core.h"

struct JSON_OBJECT {
	char *name;
	void *data;
	int data_type;
	struct JSON_OBJECT *children;
	struct JSON_OBJECT *next;
	struct JSON_OBJECT *last;
};

enum {
	JSON_ARRAY			= 1 << 0,
	JSON_OBJECT_VALUE	= 1 << 1,
	JSON_STRING			= 1 << 2,
	JSON_INT			= 1 << 3,
	JSON_BOOL			= 1 << 4,
	JSON_ARRAY_INDEX	= 1 << 5
};

typedef struct JSON_OBJECT JSON_OBJECT;

JSON_OBJECT* parse_json_from_string(char *str);
int parse_json_children(JSON_OBJECT **head, char *body, int start_index);
void initialize_json_object(JSON_OBJECT **to_init);

void set_name_json_object(JSON_OBJECT **json_object, char *name);
void set_data_json_object(JSON_OBJECT **json_object, char *name);
void insert_into_json_list(JSON_OBJECT *json_object, char *name, char *data);
void free_json_list(JSON_OBJECT *head);
JSON_OBJECT* pull_key_value_pairs_from_json_object(char *json_string);

#ifdef __cplusplus
}
#endif

#endif
