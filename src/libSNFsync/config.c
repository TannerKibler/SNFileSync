#include "config.h"

static char SYS_ID[7]     = "sys_id";
static char FILE_NAME[10] = "file_name";
static char TABLE[13]     = "target_table";
static char RECORD[14]    = "target_record";
static char TYPE[5]       = "type";
static char SUBTYPE[8]    = "subtype";
static char ACTION[8]     = "actions";
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
	else
		return -1;

	return key_hit;
}

void push_all_files() {
	/*
	   FOR ALL CHILDREN (EXCLUDING THE LANDING DIR)
	   ITERATE AND READ THE config.txt FILES INTO A SINGLY LINKED LIST OF PUSH_CONFIGS

	   THEN, CHECK FILES IN LANDING BY COMPARING AGAINST FILTER IN EACH PUSH_CONFIG
	   FOR EACH HIT:
	   IF PREPEND:
	   COPY BLOCKS FOR HEADER FILE (IF IT EXISTS) INTO THE NEW FILE WITH THE NAME BEING THE FILE NAME IN PUSH_CONFIG

	   COPY BLOCKS FOR FILE IN LANDING THAT MATCHED OUR FILTER

	   IF APPEND:
	   COPY BLOCKS FOR APPENDING FILE (IF IT EXISTS) INTO THE NEW FILE WITH THE NAME BEING THE FILE NAME IN PUSH_CONFIG

	   AFTER FILE IS COPIED, CALL PUSH IT TO INSTANCE USING THE SYS_ID OF THE PARENT DIR AND THE DATA SOURCE TABLE NAME
	 */
}

SN_INSTANCE* initialize_sn_instance() {
	SN_INSTANCE *to_init = NULL;
	to_init = malloc(sizeof(SN_INSTANCE));
	if (to_init == NULL) {
		//implement error library
		return NULL;
	}

	to_init->username	= NULL;
	to_init->password	= NULL;
	to_init->host_name	= NULL;
	to_init->next		= NULL;

	return to_init;
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

void load_sn_source_record_from_child_config(SN_SOURCE_RECORD **to_load, char *path_to_config) {
	char *file_buffer = NULL, *key = NULL, *value = NULL, *tmp = NULL;
	register int i = 0;
	int reading_value = 0, back_index = 0;

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
			tmp = realloc(value, (i - back_index)*sizeof(char));	
			if (!tmp) {
				//implement error library
				return;
			}
			value = tmp;
			value = substring(file_buffer, back_index+1, i);
			if (strncmp(key, "sys_id", 6) == 0)
				(*to_load)->sys_id = value;
			else if (strncmp(key, "table", 5) == 0)
				(*to_load)->table = value;
			else if (strncmp(key, "record", 6) == 0)
				(*to_load)->record = value;
			else if (strncmp(key, "type", 4) == 0)
				(*to_load)->type = value;
			else if (strncmp(key, "subtype", 7) == 0)
				(*to_load)->subtype = value;
			else if (strncmp(key, "file_name", 9) == 0)
				(*to_load)->file_name = value;
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
				first_sn_instance_record_in_list = initialize_sn_instance();
				if (!first_sn_instance_record_in_list) {
					//implement error library
					return NULL;
				}
				current = first_sn_instance_record_in_list;
			}
			else {
				current->next = initialize_sn_instance();
				current = current->next;
			}

			load_sn_instance_from_child_config_file(&current, files[i]);
			//printf("\nLoaded instance: %s\n", current->host_name);
			//printf("\nLoaded username: %s\n", current->username);
			//printf("\nLoaded password: %s\n", current->password);
		}
		i++;
	}

	return NULL;
}

SN_SOURCE_RECORD* load_sources_from_sn_instance() {
	SN_SOURCE_RECORD *first = NULL, *current = NULL;
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
					first_sn_source_record_in_list = initialize_sn_source_record();
					if (!first_sn_source_record_in_list) {
						//implement error library
						return NULL;
					}
					current = first_sn_source_record_in_list;
				}
				else {
					current->next = initialize_sn_source_record();
					current = current->next;
				}

				strcpy(full_path_buffer, path_buffer);
				strcat(full_path_buffer, "/");
				strcat(full_path_buffer, files[i]);
				strcat(full_path_buffer, "/");
				strcat(full_path_buffer, "config.txt");

				load_sn_source_record_from_child_config(&current, full_path_buffer);
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

	if (ensure_dir_exists(buffer, instance, "") == -1) {
		//implement error library
		return;
	}

	if (ensure_file_exists(buffer, instance, "config.txt") == -1) {
		//implement error here
		return;
	}

	if (ensure_dir_exists(buffer, "landing", "") == -1) {
		// implement error here
		return;
	}

	SN_INSTANCE *sn_instance = NULL;
	sn_instance = initialize_sn_instance();
	sn_instance->host_name = instance;
	sn_instance->username = username;
	sn_instance->password = password;

	generate_file_for_instance_config(sn_instance, sn_instance->host_name, "config.txt");

	SN_SOURCE_RECORD* tmp = NULL;
	tmp = initialize_sn_source_record();

	char *instance_dir = NULL;
	instance_dir = malloc(8);
	if (!instance_dir) {
		//implement error library
		return;
	}	

	tmp = pull_sources_from_instance(sn_instance);
	while(tmp) {

		printf("\n-----------\nSys ID: %s", (char *)tmp->sys_id);
		printf("\nTable: %s", tmp->table);
		printf("\nRecord: %s", tmp->record);
		printf("\nFile Name: %s", tmp->file_name);
		printf("\nType: %s", tmp->type);
		printf("\nSubType: %s\n---------------\n", tmp->subtype);
		instance_dir = realloc(instance_dir, (strlen(instance)*sizeof(char)) + 33);
		if (!instance_dir) {
			//implement error library
			return;
		}
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
		if (ensure_dir_exists(buffer, instance_dir, "") == -1) {
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
	free(instance_dir);
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

SN_SOURCE_RECORD* initialize_sn_source_record() {
	SN_SOURCE_RECORD *source_record = NULL;
	source_record = malloc((sizeof(SN_SOURCE_RECORD)) + 8);
	if (source_record == NULL) {
		// Implement error functionality
		return NULL;
	}

	source_record->sys_id    = NULL;
	source_record->file_name = NULL;
	source_record->next      = NULL;
	source_record->table     = NULL;
	source_record->record    = NULL;
	source_record->type      = NULL;
	source_record->subtype   = NULL;

	return source_record;
}

SN_SOURCE_RECORD* parse_returned_sn_source_records(char* sources) {
	printf("Got from instance: %s\n", sources);

	JSON_OBJECT *looper = NULL;
	SN_SOURCE_RECORD *first = NULL, *current = NULL;
	int key_result = -1;

	first_json_object_in_list = parse_json_from_string(sources);
	if (first_json_object_in_list) {
		printf("\nFirst JSON Level Name: %s\n", first_json_object_in_list->name);
		printf("\nFirst JSON level Value: %s\n", (char *)first_json_object_in_list->data);
		printf("\nStrlen of name: %li\n", strlen(first_json_object_in_list->name));
		if (strncmp(first_json_object_in_list->name, "result\0", 6) == 0) {
			printf("\nReading children of result\n");
			if (first_json_object_in_list->children) {
				looper = initialize_json_object();
				looper = first_json_object_in_list->children;
				while(looper) {
					printf("\nHad Child JSON Name: %s\n", looper->name);
					printf("\nHad Child JSON Value: %s\n", (char *)looper->data);
					if (!first) {
						first = initialize_sn_source_record();
						if (!first) {
							//implement error library
							return NULL;
						}
					}

					current = first;

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
					}
					if (is_sn_source_record_complete(current)) {
						current->next = initialize_sn_source_record();
						current = current->next;
						current->next = NULL;
					}
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

PUSH_CONFIG* initialize_push_config() {
	PUSH_CONFIG *ps = NULL;
	ps = malloc((sizeof(PUSH_CONFIG)) + 8);
	if (ps == NULL) {
		// Implement error functionality
		return NULL;
	}

	ps->next = NULL;

	return ps;
}

void set_end_file_name(PUSH_CONFIG **ps, char *name) {
	//	(*ps)->file_name = malloc(sizeof(char) * strlen(name) + 4);
	//	if ((*ps)->file_name == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*ps)->file_name = name;
}

void set_prepend_file(PUSH_CONFIG **ps, char *name) {
	//	(*ps)->file_to_prepend = malloc(sizeof(char) * strlen(name) + 4);
	//	if ((*ps)->file_to_prepend == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*ps)->file_to_prepend = name;
}

void set_append_file(PUSH_CONFIG **ps, char *name) {
	//	(*ps)->file_to_append = malloc(sizeof(char) * strlen(name) + 4);
	//	if ((*ps)->file_to_append == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*ps)->file_to_append = name;
}
void set_file_filter(PUSH_CONFIG **ps, char *filter) {
	//	(*ps)->file_name_filter = malloc(sizeof(char) * strlen(filter) + 4);
	//	if ((*ps)->file_name_filter == NULL) {
	//		// Implement Error functionality
	//		return;
	//	}

	(*ps)->file_name_filter = filter;
}
