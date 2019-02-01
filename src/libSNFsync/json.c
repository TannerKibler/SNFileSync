#include "json.h"

JSON_OBJECT* parse_json_from_string(char *json_string) {
	JSON_OBJECT *first = NULL, *current_parent = NULL, *placeholder = NULL;
	register int i = 0, reading_value = 0, next_index = 0;
	char *key = NULL, *value = NULL, *tmp = NULL;
	int shared_children_of_parent = 0, single_child_of_parent = 0, skip_data = 0;

	value = malloc(8);
	key   = malloc(8);

	if (json_string == NULL) {
		// implement error library
		return NULL;
	}

	placeholder = initialize_json_object();
	if (!placeholder) {
		//implement error library
		return NULL;
	}

	for (; json_string[i+1] != '\0'; i++) {
		if (json_string[i] == ':' || json_string[i] == ',')
			continue;

		if (json_string[i] == '[' && json_string[i+1] == '{') {
			reading_value = 0;
			shared_children_of_parent = 1;
			strcpy(value, "parent\0");
			set_data_json_object(&placeholder, value);
			skip_data = 1;
			value = NULL;
		}
		if (json_string[i] == '}' && json_string[i+1] == ']') {
			reading_value = 0;
			shared_children_of_parent = 0;
		}
		if (likely(reading_value == 1)) {
			if (json_string[i] == '\"') {
				i++;
				if (json_string[i] == '\"') {
					reading_value = 0;
					continue;
				}
				next_index = find_index_of_next_occurence('\"', i, json_string);
				if (next_index > -1) {
					next_index--;
					tmp = realloc(value, sizeof(char)*(next_index-i));
					if (!tmp) {
						// implement error library
						return NULL;
					}
					value = tmp;
					value = substring(json_string, i, next_index);
					reading_value = 0;
					printf("Had value %s", value);
					set_data_json_object(&placeholder, value);
					i = next_index + 2;
					value = NULL;
					free(value);
					free(tmp);
				}
			}
		}
		else {
			if (json_string[i] == '\"') {
				i++;
				next_index = find_index_of_next_occurence('\"', i, json_string);
				if (next_index > -1) {
					next_index--;
					tmp = realloc(key, sizeof(char)*(next_index-i));
					if (!tmp) {
						// implement error library
						return NULL;
					}
					key = tmp;
					key = substring(json_string, i, next_index);
					reading_value = 1;
					printf("Had key %s", key);
					set_name_json_object(&placeholder, key);
					i = next_index + 2;
					key = NULL;
					free(key);
					free(tmp);
				}
			}
		}

		if (placeholder && placeholder->name && (placeholder->data || skip_data)) {
			skip_data = 0;
			if (!first) {
				first = initialize_json_object();
				if (!first) {
					//implement error library
					return NULL;
				}

				set_name_json_object(&first, placeholder->name);
				set_data_json_object(&first, placeholder->data);
				if (single_child_of_parent || shared_children_of_parent)
					current_parent = first;
			}
			else if (single_child_of_parent || shared_children_of_parent) {
				if (!current_parent) {
					single_child_of_parent 		= 0;
					shared_children_of_parent 	= 0;
					continue;
				}

				if (current_parent->children == NULL) {
					current_parent->children = initialize_json_object();
					set_name_json_object(&current_parent->children, placeholder->name);
					set_data_json_object(&current_parent->children, placeholder->data);
					current_parent->children->next = NULL;
					current_parent->children->last = NULL;
					current_parent->children->children = NULL;
				}
				else {
					insert_into_json_list(current_parent->children, placeholder->name, placeholder->data);
				}

			}
			else {
				insert_into_json_list(first, placeholder->name, placeholder->data);
			}

			placeholder->name = NULL;
			placeholder->data = NULL;
		}
	}

	free(placeholder);
	//free(current_parent);
	//	free(value);
	//	free(key);
	//	free(tmp);
	return first;
}

JSON_OBJECT* initialize_json_object() {
	JSON_OBJECT *to_initialize = NULL;
	to_initialize = malloc(sizeof(JSON_OBJECT));
	if (!to_initialize) {
		//implement error library
		return NULL;
	}

	to_initialize->name		= NULL;
	to_initialize->data 	= NULL;
	to_initialize->children = NULL;
	to_initialize->next 	= NULL;
	to_initialize->last 	= NULL;

	return to_initialize;
}

void insert_into_json_list(JSON_OBJECT *parent, char *name, char *data) {
	if (!parent || !name) {
		//implement error library
		return;
	}

	JSON_OBJECT *looper = NULL;

	looper = parent;
	while(looper->next != NULL)
		looper = looper->next;

	looper->next = initialize_json_object();
	set_name_json_object(&looper->next, name);
	set_data_json_object(&looper->next, data);
	looper->next->next = NULL;
	looper->next->last = looper;
	looper->next->children = NULL;
}

void set_name_json_object(JSON_OBJECT **json_object, char *name) {
	//	(*json_object)->name = malloc(sizeof(char) * strlen(name) + 4);
	//	if ((*json_object)->name == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*json_object)->name = name;
}

void set_data_json_object(JSON_OBJECT **json_object, char *data) {
	//	(*json_object)->data = malloc(sizeof(char) * strlen(data) + 4);
	//	if ((*json_object)->data == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*json_object)->data = data;
}

void free_json_list(JSON_OBJECT *head) {
	JSON_OBJECT *swap = NULL;

	while(head != NULL) {
		free_json_list(head->children);
		swap = head;
		head = head->next;
		free(swap->name);
		free(swap->data);
		free(swap);
	}
}
