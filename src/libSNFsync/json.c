#include "json.h"

JSON_OBJECT* parse_json_from_string(char *json_string) {
	JSON_OBJECT *first = NULL, *current_parent = NULL, *placeholder = NULL;
	register int i = 0, reading_value = 0, next_index = 0;
	char *key = NULL, *value = NULL;
	int shared_children_of_parent = 0, single_child_of_parent = 0;

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
			//starting multiple child objects
			set_data_json_object(&placeholder, "parent");
			printf("\n\nSetting up first JSON level and creating child levels\n\n");
			reading_value = 0;
			shared_children_of_parent = 1;
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
					printf("\nHad no value");
					continue;
				}
				next_index = find_index_of_next_occurence('\"', i, json_string);
				if (next_index > -1) {
					next_index--;
					if (!value) {
						value = malloc(sizeof(char)*(next_index-i));
						if (!value) {
							// implement error library
							return NULL;
						}
					}
					else {
						value = realloc(value, sizeof(char)*(next_index-i));
						if (!value) {
							// implement error library
							return NULL;
						}
					}
					value = substring(json_string, i, next_index);
					reading_value = 0;
					printf("Had value %s", value);
					set_data_json_object(&placeholder, (char *)value);
					i = next_index + 2;
					value = NULL;
				}
			}
		}
		else {
			if (json_string[i] == '\"') {
				i++;
				next_index = find_index_of_next_occurence('\"', i, json_string);
				if (next_index > -1) {
					next_index--;
					if (!key) {
						key = malloc(sizeof(char)*(next_index-i));
						if (!key) {
							// implement error library
							return NULL;
						}
					}
					else {
						key = realloc(key, sizeof(char)*(next_index-i));
						if (!key) {
							// implement error library
							return NULL;
						}
					}
					key = substring(json_string, i, next_index);
					reading_value = 1;
					printf("Had key %s", key);
					set_name_json_object(&placeholder, key);
					i = next_index + 2;
					key = NULL;
				}
			}
		}

		if (placeholder && placeholder->name && placeholder->data) {
			if (!first) {
				first = initialize_json_object();
				if (!first) {
					//implement error library
					return NULL;
				}

				set_name_json_object(&first, placeholder->name);
				set_data_json_object(&first, (char *)placeholder->data);
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
					set_data_json_object(&current_parent->children, (char *)placeholder->data);
					current_parent->children->next = NULL;
					current_parent->children->last = NULL;
					current_parent->children->children = NULL;
				}
				else {
					insert_into_json_list(current_parent->children, placeholder->name, (char *)placeholder->data);
				}

			}
			else {
				insert_into_json_list(first, placeholder->name, (char *)placeholder->data);
			}

			placeholder->name = NULL;
			placeholder->data = NULL;
		}
	}

	free(placeholder);
	//free(current_parent);
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

//	if (parent->children == NULL) {
//		parent->children = initialize_json_object();
//		set_name_json_object(&parent->children, name);
//		set_data_json_object(&parent->children, data);
//		parent->children->next = NULL;
//		parent->children->last = NULL;
//		parent->children->children = NULL;
//		return;
//	}
//	else {
		looper = parent;
		while(looper->next != NULL)
			looper = looper->next;

		looper->next = initialize_json_object();
		set_name_json_object(&looper->next, name);
		set_data_json_object(&looper->next, data);
		looper->next->next = NULL;
		looper->next->last = looper;
		looper->next->children = NULL;
	//}
}

void set_name_json_object(JSON_OBJECT **json_object, char *name) {
	(*json_object)->name = malloc(sizeof(char) * strlen(name) + 4);
	if ((*json_object)->name == NULL) {
		// Implement Error functionality
		return;
	}

	(*json_object)->name = name;
}

void set_data_json_object(JSON_OBJECT **json_object, char *data) {
	(*json_object)->data = malloc(sizeof(char) * strlen(data) + 4);
	if ((*json_object)->data == NULL) {
		// Implement Error functionality
		return;
	}

	(*json_object)->data = data;
}
