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
	struct JSON_OBJECT *children;
	struct JSON_OBJECT *next;
	struct JSON_OBJECT *last;
};

typedef struct JSON_OBJECT JSON_OBJECT;

JSON_OBJECT* parse_json_from_string(char *str);
JSON_OBJECT* initialize_json_object();

void set_name_json_object(JSON_OBJECT **json_object, char *name);
void set_data_json_object(JSON_OBJECT **json_object, char *name);
void insert_into_json_list(JSON_OBJECT *json_object, char *name, char *data);

#ifdef __cplusplus
}
#endif

#endif
