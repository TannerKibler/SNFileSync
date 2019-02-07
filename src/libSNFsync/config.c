#include "config.h"

//static char SYS_ID[7]     = "sys_id";
//static char FILE_NAME[10] = "file_name";
//static char TABLE[13]     = "target_table";
//static char RECORD[14]    = "target_record";
//static char TYPE[5]       = "type";
//static char SUBTYPE[8]    = "subtype";
//static char ACTION[8]     = "actions";

static SN_SOURCE_RECORD *first_sn_source_record_in_list = NULL;
static JSON_OBJECT *first_json_object_in_list = NULL;
static SN_INSTANCE *first_sn_instance_record_in_list = NULL;

int seek_keys(char *str) {
	int key_hit = 0;

	if (str == NULL)
		return -1;

	if (strncmp(str, SYS_ID, 6) == 0)
		key_hit = 0;
	else if (strncmp(str, FILE_NAME, 9) == 0)
		key_hit = 1;
	else if (strncmp(str, TABLE, 12) == 0)
		key_hit = 2;
	else if (strncmp(str, RECORD, 13) == 0)
		key_hit = 3;
	else if (strncmp(str, TYPE, 4) == 0)
		key_hit = 4;
	else if (strncmp(str, SUBTYPE, 7) == 0)
		key_hit = 5;
	else if (strncmp(str, FILE_TYPE, 9) == 0)
		key_hit = 6;
	else if (strncmp(str, ACTION, 7) == 0)
		key_hit = 7;
	else
		return -1;

	return key_hit;
}

void initialize_sn_instance(SN_INSTANCE **to_init) {
	//	SN_INSTANCE *to_init = NULL;
	//	to_init = malloc(sizeof(SN_INSTANCE));
	//	if (to_init == NULL) {
	//		//implement error library
	//		return NULL;
	//	}

	(*to_init)->username	= NULL;
	(*to_init)->password	= NULL;
	(*to_init)->host_name	= NULL;
	(*to_init)->next		= NULL;

	//return to_init;
}

void load_sn_instance_from_child_config_file(SN_INSTANCE **to_load, char *instance_dir_name) {
	char path_buffer[MAX_PATH];
	char *file_buffer = NULL, *key = NULL, *value = NULL, *tmp = NULL;
	register int i = 0;
	int reading_value = 0, back_index = 0;
	get_current_directory(path_buffer);
	strcat(path_buffer, "/");
	strcat(path_buffer, instance_dir_name);
	strcat(path_buffer, "/");
	strcat(path_buffer, "config.txt");

	key = malloc(8);
	value = malloc(8);
	if (!key || !value) {
		//implement error library
		return;
	}

	file_buffer = read_file_to_buffer(path_buffer);
	while (file_buffer[i] != '\0') {
		if (file_buffer[i] == '\n') {
			reading_value = 0;
		}

		if (reading_value == 1 && (file_buffer[i+1] == '\n' || file_buffer[i+1] == '\0')) {
			back_index = backwards_find_index_from_index(file_buffer, ':', i);
			tmp = realloc(value, (i - back_index)*sizeof(char));	
			if (!tmp) {
				//implement error library
				return;
			}
			value = tmp;
			value = substring(file_buffer, back_index+1, i);
			if (strncmp(key, "username", 8) == 0)
				(*to_load)->username = value;
			else if (strncmp(key, "password", 8) == 0)
				(*to_load)->password = value;
			else if (strncmp(key, "instance", 8) == 0)
				(*to_load)->host_name = value;
		}

		if (reading_value == 0 && file_buffer[i] == ':') {
			reading_value = 1;
			back_index = backwards_find_index_from_index(file_buffer, '\n', i);
			tmp = realloc(key, (i - back_index)*sizeof(char));	
			if (!tmp) {
				//implement error library
				return;
			}
			key = tmp;
			if (back_index == -1)
				key = substring(file_buffer, 0, i-1);
			else
				key = substring(file_buffer, back_index+1, i-1);
		}

		i++;
	}
}

SN_SOURCE_RECORD* get_first_sn_source_record() {
	if (first_sn_source_record_in_list == NULL) 
		load_sources_from_sn_instance();


	return first_sn_source_record_in_list; 
}

void load_action_from_string(SN_ACTION **to_load, char *action_string) {
	int i = 0, reading_value = 0, start_index = 0;
	char *key = NULL, *tmp = NULL;
	
	while (action_string[i] != '\0' && action_string[i] != '\n') {
		if (reading_value == 0) {
			if (action_string[i] == '=') {
				key = substring(action_string, start_index, i-1);
				reading_value = 1;
				start_index = i+1;
			}
		}
		else {
			if (action_string[i] == ',') {
				if (strncmp(key, "name", 4) == 0) {
					(*to_load)->action = malloc(sizeof(char)*strlen(key));
					if (!(*to_load)->action) {
						//implement error library
						return;
					}
					tmp = substring(action_string, start_index, i-1);
					(*to_load)->action = tmp;
					free(key);
				}
				else if (strncmp(key, "lines", 5) == 0) {
					(*to_load)->lines = malloc(sizeof(char)*strlen(key));
					if (!(*to_load)->lines) {
						//implement error library
						return;
					}
					tmp = substring(action_string, start_index, i-1);
					(*to_load)->lines = tmp;
					free(key);
				}
				reading_value = 0;
				start_index = i+1;
			}
		}
		i++;
	}
}

void load_sn_source_record_from_child_config(SN_SOURCE_RECORD **to_load, char *path_to_config) {
	char *file_buffer = NULL, *key = NULL, *value = NULL, *tmp = NULL;
	register int i = 0;
	int reading_value = 0, back_index = 0, next_index = 0;
	SN_ACTION *action = NULL, *first = NULL, *loop = NULL;

	key = malloc(8);
	value = malloc(8);
	if (!key || !value) {
		//implement error library
		return;
	}

	file_buffer = read_file_to_buffer(path_to_config);
	while (file_buffer[i] != '\0') {
		if (file_buffer[i] == '\n') {
			reading_value = 0;
		}

		if (reading_value == 1 && (file_buffer[i+1] == '\n' || file_buffer[i+1] == '\0')) {
			back_index = backwards_find_index_from_index(file_buffer, ':', i);
			
			if (back_index == -1) back_index = 0;
			else back_index = back_index+1;

			value = substring(file_buffer, back_index, i);
			printf("\nLoading key from file: %s\n", key);
			printf("\nLoading value from file: %s\n", value);
			if (strncmp(key, "sys_id", 6) == 0) {
				(*to_load)->sys_id = malloc(33);
				if (!(*to_load)->sys_id) {
					//implement error library
					return;
				}
				strcpy((*to_load)->sys_id, value);
			}
			else if (strncmp(key, "table", 5) == 0) {
				(*to_load)->table = malloc(sizeof(char)*strlen(value)+1);
				if (!(*to_load)->table) {
					//implement error library
					return;
				}
				strcpy((*to_load)->table, value);
			}
			else if (strncmp(key, "record", 6) == 0) {
				(*to_load)->record = malloc(sizeof(char)*strlen(value)+1);
				if (!(*to_load)->record) {
					//implement error library
					return;
				}
				strcpy((*to_load)->record, value);
			}
			else if (strncmp(key, "type", 4) == 0) {
				(*to_load)->type = malloc(sizeof(char)*strlen(value)+1);
				if (!(*to_load)->type) {
					//implement error library
					return;
				}
				strcpy((*to_load)->type, value);
			}
			else if (strncmp(key, "subtype", 7) == 0) {
				(*to_load)->subtype = malloc(sizeof(char)*strlen(value)+1);
				if (!(*to_load)->subtype) {
					//implement error library
					return;
				}
				strcpy((*to_load)->subtype, value);
			}
			else if (strncmp(key, "file_name", 9) == 0) {
				(*to_load)->file_name = malloc(sizeof(char)*strlen(value)+1);
				if (!(*to_load)->file_name) {
					//implement error library
					return;
				}
				strcpy((*to_load)->file_name, value);
			}
			else if (strncmp(key, "file_type", 9) == 0) {
				(*to_load)->file_type = malloc(sizeof(char)*strlen(value)+1);
				if (!(*to_load)->file_type) {
					//implement error library
					return;
				}
				strcpy((*to_load)->file_type, value);
			}
			else if (strncmp(key, "action", 6) == 0) {
				first = malloc(sizeof(sizeof(SN_ACTION)));
				if (!first) {
					//implement error library
					return;
				}

				next_index = find_index_of_next_occurence(';', back_index, file_buffer);
				if (next_index == -1) {
					free(first);
				}
				tmp = substring(file_buffer, back_index, next_index-1);
				load_action_from_string(&first, tmp);
				first->next = NULL;
				while(next_index != i) {
					action = malloc(sizeof(sizeof(SN_ACTION)));
					if (!action) {
						//implement error library
						return;
					}
					back_index = next_index;
					next_index = find_index_of_next_occurence(';', back_index, file_buffer);
					tmp = substring(file_buffer, back_index+1, next_index-1);
					load_action_from_string(&action, tmp);
					action->next = NULL;
					if (!first->next) {
						first->next = action;
						continue;
					}
					loop = first->next;
					while(loop) loop = loop->next;
					loop = action;
				}

				(*to_load)->action = first;
			}
		}

		if (reading_value == 0 && file_buffer[i] == ':') {
			reading_value = 1;
			back_index = backwards_find_index_from_index(file_buffer, '\n', i);

			tmp = realloc(key, (i - back_index)*sizeof(char));	
			if (!tmp) {
				//implement error library
				return;
			}
			key = tmp;
			if (back_index == -1)
				key = substring(file_buffer, 0, i-1);
			else
				key = substring(file_buffer, back_index+1, i-1);
		}

		i++;
	}

}

int check_for_child_config_file(char *dir_name, char *prepend) {
	char path_buffer[MAX_PATH];
	//get_current_directory(path_buffer);
	strcpy(path_buffer, dir_name);
	if (prepend) {
		strcat(path_buffer, "/");
		strcat(path_buffer, prepend);
	}
	strcat(path_buffer, "/");
	strcat(path_buffer, "config.txt");

	//printf("\n ---------\nChecking for config.txt file here: %s\n-------------\n", path_buffer);

	if (access(path_buffer, F_OK ) != -1 ) {
		return 0;
	}

	return -1;
}

SN_INSTANCE* load_instances_from_files() {
	char path_buffer[MAX_PATH];
	SN_INSTANCE *current = NULL;
	int i = 0;
	char ** files = NULL;

	get_current_directory(path_buffer);	
	files = read_files_in_directory(path_buffer);

	while(files[i]) {
		if ((strncmp(files[i], ".", 1) == 0) || (strncmp(files[i], "..", 2) == 0)) {
			i++;
			continue;
		}
		if (check_for_child_config_file(path_buffer, files[i]) == 0) {
			if (!first_sn_instance_record_in_list) {
				first_sn_instance_record_in_list = malloc(sizeof(SN_INSTANCE));
				if (!first_sn_instance_record_in_list) {
					//implement error library
					return NULL;
				}
				initialize_sn_instance(&first_sn_instance_record_in_list);
				if (!first_sn_instance_record_in_list) {
					//implement error library
					return NULL;
				}
				current = first_sn_instance_record_in_list;
			}
			else {
				current->next = malloc(sizeof(SN_INSTANCE));
				if (!current->next) {
					//implement error library
					return NULL;
				}
				initialize_sn_instance(&current->next);
				current = current->next;
			}

			load_sn_instance_from_child_config_file(&current, files[i]);
			printf("\nLoaded instance: %s\n", current->host_name);
			printf("\nLoaded username: %s\n", current->username);
			//printf("\nLoaded password: %s\n", current->password);
		}
		i++;
	}

	return NULL;
}

void call_send_to_instance() {
	//post_attachment_to_servicenow("/home/arch/code/c/SNFileSync/misc/test_file", first_sn_source_record_in_list);
}

SN_SOURCE_RECORD* load_sources_from_sn_instance() {
	SN_SOURCE_RECORD *current = NULL;
	int i = 0;
	char ** files = NULL;
	SN_INSTANCE *looper = NULL;
	looper = first_sn_instance_record_in_list;
	if (looper == NULL)
		load_instances_from_files();

	looper = first_sn_instance_record_in_list;

	while(looper) {
		char path_buffer[MAX_PATH], full_path_buffer[MAX_PATH];
		get_current_directory(path_buffer);
		strcat(path_buffer, "/");
		strcat(path_buffer, looper->host_name);
		files = read_files_in_directory(path_buffer);

		while(files[i]) {
			if ((strncmp(files[i], ".", 1) == 0) || (strncmp(files[i], "..", 2) == 0) || (strncmp(files[i], "config.txt", 10) == 0)) {
				i++;
				continue;
			}
			if (check_for_child_config_file(path_buffer, files[i]) == 0) {
				if (!first_sn_source_record_in_list) {
					first_sn_source_record_in_list = malloc(sizeof(SN_SOURCE_RECORD));
					if (!first_sn_source_record_in_list) {
						//implement error library
						return NULL;
					}
					initialize_sn_source_record(&first_sn_source_record_in_list);
					current = first_sn_source_record_in_list;
				}
				else {
					current->next = malloc(sizeof(SN_SOURCE_RECORD));
					if (!current->next) {
						//implement error library
						return NULL;
					}
					initialize_sn_source_record(&current->next);
					current = current->next;
				}

				strcpy(full_path_buffer, path_buffer);
				strcat(full_path_buffer, "/");
				strcat(full_path_buffer, files[i]);
				strcat(full_path_buffer, "/");
				strcat(full_path_buffer, "config.txt");

				load_sn_source_record_from_child_config(&current, full_path_buffer);
				current->instance = looper;
				//current->instance = malloc(sizeof(looper->host_name) + 4);
				//strcpy(current->host_name, looper->host_name);
				//printf("\nLoaded table: %s\n", current->table);
				//printf("\nLoaded record: %s\n", current->record);
				//printf("\nLoaded file_name: %s\n", current->file_name);
				//printf("\nLoaded sys_id: %s\n", current->sys_id);
				//printf("\nLoaded type: %s\n", current->type);
				//printf("\nLoaded subtype: %s\n", current->subtype);
			}
			i++;
		}
		looper = looper->next;
	}

	return first_sn_source_record_in_list;
}

void generate_config_files(char* instance, char *username, char *password) {
	if (instance == NULL || username == NULL || password == NULL)
		return;

	char buffer[MAX_PATH];
	char *instance_dir = NULL, *tmp_to_realloc = NULL;
	get_current_directory(buffer);

	if (ensure_dir_exists(buffer, instance, NULL) == -1) {
		//implement error library
		return;
	}

	if (ensure_file_exists(buffer, instance, "config.txt") == -1) {
		//implement error here
		return;
	}

	if (ensure_dir_exists(buffer, "landing", NULL) == -1) {
		// implement error here
		return;
	}

	printf("\nHad buffer before generate: %s\n", buffer);

	SN_INSTANCE *sn_instance = NULL;
	first_sn_instance_record_in_list = malloc(sizeof(SN_INSTANCE));
	if (!first_sn_instance_record_in_list) {
		//implement error library
		return;
	}
	sn_instance = first_sn_instance_record_in_list;
	initialize_sn_instance(&sn_instance);
	sn_instance->host_name = instance;
	sn_instance->username = username;
	sn_instance->password = password;

	generate_file_for_instance_config(sn_instance, sn_instance->host_name, "config.txt\0");

	SN_SOURCE_RECORD* tmp = NULL;
	instance_dir = malloc(8);
	if (!instance_dir) {
		//implement error library
		return;
	}	

	tmp = pull_sources_from_instance(sn_instance);
	while(tmp) {

		//printf("\n-----------\nSys ID: %s", (char *)tmp->sys_id);
		//printf("\nTable: %s", tmp->table);
		//printf("\nRecord: %s", tmp->record);
		//printf("\nFile Name: %s", tmp->file_name);
		//printf("\nType: %s", tmp->type);
		//printf("\nSubType: %s\n---------------\n", tmp->subtype);
		//printf("\nAction:\n");
		//printf("\nName: %s\n", tmp->action->action);
		//printf("\nOrder: %s\n", tmp->action->order);
		tmp_to_realloc = realloc(instance_dir, (strlen(instance)*sizeof(char)) + 33);
		if (!tmp_to_realloc) {
			//implement error library
			return;
		}
		instance_dir = tmp_to_realloc;
		instance_dir = instance;
		instance_dir = realloc(instance_dir, (strlen(instance_dir)*sizeof(char)) + 34);
		if (!instance_dir) {
			//implement error library
			break;
		}
		printf("\nHad Instance: %s\n", instance_dir);
#ifdef WINDOWS
		strcat(instance_dir, "\\");
#else
		strcat(instance_dir, "/");
#endif
		strcat(instance_dir, tmp->sys_id);
		if (ensure_dir_exists(buffer, instance_dir, NULL) == -1) {
			// implement error here
			break;
		}
		if (ensure_dir_exists(buffer, instance_dir, "success") == -1) {
			// implement error here
			break;
		}
		if (ensure_file_exists(buffer, instance_dir, "config.txt") == -1) {
			//implement error here
			break;
		}

		generate_config_file_from_source_record(tmp, instance_dir, "config.txt\0");
		tmp = tmp->next;
	}

	free_source_list(first_sn_source_record_in_list);
	free_json_list(first_json_object_in_list);
	free_instance_list(first_sn_instance_record_in_list);
	free(instance_dir);
	free(tmp_to_realloc);
	if (first_sn_source_record_in_list == NULL)
		printf("\nSuccessfully freed source list\n");
}

void free_source_list(SN_SOURCE_RECORD *head) {
	SN_SOURCE_RECORD *swap = NULL;
	printf("\n-------------------------------------\nFreeing Source List\n---------------------------\n");

	while (head != NULL) {
		printf("\n Freeing source node \n");
		swap = head;
		head = head->next;
		//free(swap->sys_id);
		//free(swap->file_name);
		//free(swap->table);
		//free(swap->record);
		//free(swap->type);
		//free(swap->subtype);
		free(swap);
	}
}

SN_SOURCE_RECORD* pull_sources_from_instance(SN_INSTANCE* instance) {
	//SN_SOURCE_RECORD *ds = NULL;
	load_sn_source_records(instance); //DEFAULTING THIS UNTIL I FIGURE OUT CURL LINKING ISSUES
	if (first_sn_source_record_in_list == NULL)
		return NULL;

	return first_sn_source_record_in_list;
}

void initialize_sn_source_record(SN_SOURCE_RECORD **source_record) {
	//SN_SOURCE_RECORD *source_record = NULL;
	//source_record = malloc((sizeof(SN_SOURCE_RECORD)) + 8);
	//if (source_record == NULL) {
	//	// Implement error functionality
	//	return NULL;
	//}

	(*source_record)->sys_id    = NULL;
	(*source_record)->file_name = NULL;
	(*source_record)->next      = NULL;
	(*source_record)->table     = NULL;
	(*source_record)->record    = NULL;
	(*source_record)->type      = NULL;
	(*source_record)->subtype   = NULL;
	(*source_record)->file_type = NULL;
	(*source_record)->instance  = NULL;
}

SN_SOURCE_RECORD* parse_returned_sn_source_records(char* sources) {
	printf("Got from instance: %s\n", sources);

	JSON_OBJECT *looper = NULL, *nested_looper = NULL, *tmp = NULL;
	SN_SOURCE_RECORD *first = NULL, *current = NULL;
	SN_ACTION *first_action = NULL, *swp_action = NULL;
	int key_result = -1;

	first_json_object_in_list = parse_json_from_string(sources);
	if (first_json_object_in_list) {
		if (!first) {
			first = malloc(sizeof(SN_SOURCE_RECORD));
			if (!first) {
				//implement error library
				return NULL;
			}
			initialize_sn_source_record(&first);
		}
		current = first;

		if (first_json_object_in_list->data_type & JSON_ARRAY) {
			if (!first_json_object_in_list->children)
				return NULL;

			if (first_json_object_in_list->children->data_type & JSON_ARRAY_INDEX) {
				looper = first_json_object_in_list->children;
				while(looper) {
					if (looper->data_type & JSON_ARRAY_INDEX)
						looper = looper->children;

					printf("\nHad Child JSON Name: %s\n", looper->name);
					printf("\nHad Child JSON Value: %s\n", (char *)looper->data);

					key_result = seek_keys(looper->name);
					switch(key_result) {
						case 0:
							set_sys_sn_source_record(&current, (char *)looper->data);
							break;
						case 1:
							set_name_sn_source_record(&current, (char *)looper->data);
							break;
						case 2:
							set_table_sn_source_record(&current, (char *)looper->data);
							break;
						case 3:
							set_record_sn_source_record(&current, (char *)looper->data);
							break;
						case 4:
							set_type_sn_source_record(&current, (char *)looper->data);
							break;
						case 5:
							set_subtype_sn_source_record(&current, (char *)looper->data);
							break;
						case 6:
							//set_filetype_sn_source_record(&current, (char *)looper->data);
							current->file_type = (char *)looper->data;
							break;
						case 7:
							nested_looper = looper->children; // array indexes
							while(nested_looper) {
								tmp = nested_looper->children;
								first_action = malloc(sizeof(SN_ACTION));
								if (!first_action) {
									//implement error library
									return NULL;
								}
								first_action->next		= NULL;
								first_action->action	= NULL;
								first_action->order		= NULL;
								first_action->lines		= NULL;
								while(tmp) {
									if (!tmp->name || !tmp->data) {
										tmp = tmp->next;
										continue;
									}
									if (strncmp(tmp->name, "action", 7) == 0) {
										first_action->action = malloc(sizeof(char)*strlen((char *)tmp->data));
										strcpy(first_action->action, (char *)tmp->data);
									}
									else if (strncmp(tmp->name, "order", 5) == 0) {
										first_action->order = malloc(sizeof(char)*strlen((char *)tmp->data));
										strcpy(first_action->order, (char *)tmp->data);
									}
									else if (strncmp(tmp->name, "lines_to_remove", 15) == 0) {
										first_action->lines = malloc(sizeof(char)*strlen((char *)tmp->data));
										strcpy(first_action->lines, (char *)tmp->data);
									}
									tmp = tmp->next;
								}
								if (!current->action) {
									current->action = first_action;
								}
								else {
									swp_action = current->action;
									while(swp_action->next) swp_action = swp_action->next;
									swp_action->next = first_action;
									swp_action->next->next = NULL;
								}
								//free(first_action);
								nested_looper = nested_looper->next;
							}
							break;

					}
					if (is_sn_source_record_complete(current)) {
						current->next = malloc(sizeof(SN_SOURCE_RECORD));
						if (!current->next) {
							//implement error library
							return NULL;
						}
						initialize_sn_source_record(&current->next);
						current = current->next;
						current->next = NULL;
						if (looper->next)
							looper = looper->next;
						else if (first_json_object_in_list->children->next)
							looper = first_json_object_in_list->children->next;
					}
					else
						looper = looper->next;
				}
			}
		}
	}
	first_sn_source_record_in_list = first;
	return first_sn_source_record_in_list;
}

int is_sn_source_record_complete(SN_SOURCE_RECORD *to_test) {
	if (!to_test)
		return 0;

	if (to_test->sys_id == NULL)
		return 0;

	if (to_test->file_name == NULL)
		return 0;

	if (to_test->table == NULL)
		return 0;

	if (to_test->record == NULL)
		return 0;

	if (to_test->type == NULL)
		return 0;

	if (to_test->subtype == NULL)
		return 0;

	return 1;
}

void set_sys_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *sys) {
	//	(*sn_source_record)->sys_id = malloc(33);
	//	if ((*sn_source_record)->sys_id == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*sn_source_record)->sys_id = sys;
}

void set_name_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *name) {
	//	(*sn_source_record)->file_name = malloc(sizeof(char) * strlen(name) + 4);
	//	if ((*sn_source_record)->file_name == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*sn_source_record)->file_name = name;
}



void set_table_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *table) {
	//	(*sn_source_record)->table = malloc(sizeof(char) * strlen(table) + 4);
	//	if ((*sn_source_record)->table == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*sn_source_record)->table = table;
}

void set_record_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *record) {
	//	(*sn_source_record)->record = malloc(sizeof(char) * strlen(record) + 4);
	//	if ((*sn_source_record)->record == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*sn_source_record)->record = record;
}

void set_type_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *type) {
	//	(*sn_source_record)->type = malloc(sizeof(char) * strlen(type) + 4);
	//	if ((*sn_source_record)->type == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*sn_source_record)->type = type;
}

void set_subtype_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *subtype) {
	//	(*sn_source_record)->subtype = malloc(sizeof(char) * strlen(subtype) + 4);
	//	if ((*sn_source_record)->subtype == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*sn_source_record)->subtype = subtype;
}

void free_instance_list(SN_INSTANCE *head) {
	SN_INSTANCE *swap = NULL;
	printf("\n-------------------------------------\nFreeing Instance List\n---------------------------\n");

	while (head != NULL) {
		printf("\n Freeing instance node \n");
		swap = head;
		head = head->next;
		//free(swap->sys_id);
		//free(swap->file_name);
		//free(swap->table);
		//free(swap->record);
		//free(swap->type);
		//free(swap->subtype);
		free(swap);
	}

}
