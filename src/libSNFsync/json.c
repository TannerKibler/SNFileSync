#include "json.h"

JSON_OBJECT* pull_key_value_pairs_from_json_object(char *json_string) {
	long int i = 0;
	int reading_value = 0, next_index = 0;
	JSON_OBJECT *child = NULL, *swp = NULL, *nested_swp = NULL, *first = NULL;
	char *key = NULL, *value = NULL, *tmp = NULL;

	first = malloc(sizeof(JSON_OBJECT));
	if (!first) {
		//implement error library
		return NULL;
	}
	swp = malloc(sizeof(JSON_OBJECT));
	if (!swp) {
		//implement error library
		return NULL;
	}

	initialize_json_object(&first);
	initialize_json_object(&swp);
	child = first;

	child->name = NULL;
	child->data = NULL;
	child->next = NULL;
	child->children= NULL;

	for (; json_string[i+1] != '\0'; i++) {
		if (json_string[i] == ':' || json_string[i] == ',')
			continue;
		if (reading_value) {
			if (json_string[i] == '[') {
				child->data_type = JSON_ARRAY;
				swp = malloc(sizeof(JSON_OBJECT));
				if (!swp) {
					//implement error library
					return NULL;
				}
				initialize_json_object(&swp);
				if (json_string[i+1] == '{') {
					swp->data_type = JSON_ARRAY_INDEX;
					next_index = find_index_of_next_occurence('}', i+1, json_string);
					tmp = substring(json_string, i+1, next_index);
					swp->children = pull_key_value_pairs_from_json_object(tmp);
					if (json_string[next_index+1] != ',') {
						child->children = swp;
						i = next_index+1;
					}

					while (json_string[next_index+1] == ',') {
						i = next_index+2;
						next_index = find_index_of_next_occurence('}', i, json_string);
						nested_swp = malloc(sizeof(JSON_OBJECT));
						if (!nested_swp) {
							//implement error library
							return NULL;
						}
						initialize_json_object(&nested_swp);
						if (tmp)
							free(tmp);
						
						tmp = substring(json_string, i, next_index);
						nested_swp->children = pull_key_value_pairs_from_json_object(tmp);
						free(tmp);
						nested_swp->data_type = JSON_OBJECT_VALUE;
						swp->next = nested_swp;
						if (!child->children)
							child->children = swp;
						else
							swp = swp->next;
					}
					free(tmp);
				}
			}
			if (json_string[i] == '\"') {
				i++;
				if (json_string[i] == '\"') {
					reading_value = 0;
					continue;
				}
				next_index = find_index_of_next_occurence('\"', i, json_string);
				if (next_index > -1) {
					next_index--;
					value = substring(json_string, i, next_index);
					reading_value = 0;
					set_data_json_object(&child, value);
					i = next_index + 2;
					value = NULL;
					free(value);
				}
			}
		}
		else {
			if (json_string[i] == '\"') {
				i++;
				next_index = find_index_of_next_occurence('\"', i, json_string);
				if (next_index > -1) {
					next_index--;
					key = substring(json_string, i, next_index);
					reading_value = 1;
					set_name_json_object(&child, key);
					i = next_index + 2;
					key = NULL;
					free(key);
				}
			}
		}

		if (child->name && child->data) {
			swp = malloc(sizeof(JSON_OBJECT));
			if (!swp) {
				//implement error library
				return NULL;
			}
			initialize_json_object(&swp);
			child->next = swp;
			child = child->next;
			child->name = NULL;
			child->data = NULL;
			child->next = NULL;
			child->last = NULL;
			child->children = NULL;
		}
	}

	return first;
}

int parse_json_children(JSON_OBJECT **head, char *body, int start_index) {
	int end_index = 0, next_index = 0, i = 0;
	char *child_body = NULL, *tmp = NULL;
	JSON_OBJECT *child = NULL, *swp = NULL;

	if ((*head)->data_type & JSON_ARRAY) {
		end_index = find_index_of_next_occurence(']', start_index, body);
		if (end_index == -1)
			return -1;

		while (body[end_index+1] != ',' && body[end_index+1] != '}' && end_index != -1)
			end_index = find_index_of_next_occurence(']', end_index, body);

		if (end_index == -1)
			return -1;

		i = start_index;
		while(i <= end_index) {
			if (body[i] == '{') {
				if (!(*head)->children) {
					child = malloc(sizeof(JSON_OBJECT));
					if (!child) {
						//implement error library
						return -1;
					}
					initialize_json_object(&child);
					child->data_type = JSON_ARRAY_INDEX;
					(*head)->children = child;
					next_index = find_index_of_next_occurence('}', i, body);
					if (find_index_of_next_occurence('[', i, body) == -1) { // if no nested arrays
						if (body[next_index+1] == ',' || body[next_index+1] == ']') {
							tmp = substring(body, i, next_index);
							swp = pull_key_value_pairs_from_json_object(tmp);
							free(tmp);
						}
						if (swp)
							swp->data_type = JSON_OBJECT_VALUE;

						//swp->next = NULL;

						(*head)->children->children = swp;
					}
					else {
						if (body[next_index+1] != ']') {
							while(body[next_index+1] != ']') {
								next_index = find_index_of_next_occurence('}', next_index+1, body);
								if (body[next_index+1] == ',' || body[next_index+1] == ']') {
									tmp = substring(body, i, next_index);
									swp = pull_key_value_pairs_from_json_object(tmp);
									free(tmp);
									if (swp) {
										swp->data_type = JSON_OBJECT_VALUE;
									}

									if (!(*head)->children->children) {
										(*head)->children->children = swp;
										swp = (*head)->children->children->next;
									}
									else {
										child = (*head)->children->children->next;
										while(child)
											child = child->next;

										child = swp;
										swp = child->next;
									}

									i = next_index+1;
								}
							}
						}
					}
				}
				else {
					next_index = find_index_of_next_occurence('}', i, body);
					if (body[next_index+1] == ',' || body[next_index+1] == ']')
						child->next = pull_key_value_pairs_from_json_object(substring(body, i, next_index));
				}

				i = next_index;
			}
			i++;
			swp = NULL;
			free(swp);
		}
	}
	else if ((*head)->data_type & JSON_OBJECT_VALUE) {
		end_index = find_index_of_next_occurence('}', start_index, body);
		if (body[end_index+1] != ',' && body[end_index+1] != ']') {

		}
		else {
			child_body = substring(body, start_index, end_index);
			child = malloc(sizeof(JSON_OBJECT));
			if (!child) {
				//implement error library
				return -1;
			}

			(*head)->children = child;
		}
	}
	return end_index;
}

JSON_OBJECT* parse_json_from_string(char *json_string) {
	JSON_OBJECT *first = NULL, *placeholder = NULL;
	register long int i = 0, next_index = 0;
	char *tmp = NULL;

	if (json_string == NULL) {
		// implement error library
		return NULL;
	}

	first = malloc(sizeof(JSON_OBJECT));
	if (!first) {
		//implement error library
		return NULL;
	}

	initialize_json_object(&first);
	placeholder = first;

	for (; json_string[i+1] != '\0'; i++) {
		if (json_string[i] == ':' || json_string[i] == ',')
			continue;

		if (json_string[i] == '[') {
			placeholder->name = NULL;
			placeholder->data = NULL;
			placeholder->data_type = JSON_ARRAY;
			next_index = parse_json_children(&placeholder, json_string, i);
			if (next_index == -1) {
				//implement error library
				return NULL;
			}
			else
				i = next_index;
		}
		if (json_string[i] == '{') {
			tmp = substring(json_string, i+2, i+7);
			if (strncmp(tmp, "result", 6) == 0) {
				i = i+8;
				free(tmp);
				continue;
			}
			free(tmp);
			placeholder->name = NULL;
			placeholder->data = NULL;
			placeholder->data_type = JSON_OBJECT_VALUE;
			next_index = parse_json_children(&placeholder, json_string, i);
		}
	}

	free(placeholder);
	return first;
}

void initialize_json_object(JSON_OBJECT **to_initialize) {
	//JSON_OBJECT *to_initialize = NULL;
	//to_initialize = malloc(sizeof(JSON_OBJECT));
	//if (!to_initialize) {
	//	//implement error library
	//	return NULL;
	//}

	(*to_initialize)->name		= NULL;
	(*to_initialize)->data 	= NULL;
	(*to_initialize)->children = NULL;
	(*to_initialize)->next 	= NULL;
	(*to_initialize)->last 	= NULL;
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

	//looper->next = initialize_json_object();
	looper->next = malloc(sizeof(JSON_OBJECT));
	if (!looper->next) {
		//implement error library
		return;
	}

	initialize_json_object(&looper->next);
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
		if (swap->name)
			free(swap->name);
		if (swap->data)
			free(swap->data);
		if (swap)
			free(swap);
	}
}
