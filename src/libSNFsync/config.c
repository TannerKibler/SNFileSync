#include "config.h"

static char SYS_ID[7]     = "sys_id";
static char FILE_NAME[10] = "file_name";
static char TABLE[13]     = "target_table";
static char RECORD[14]    = "target_record";
static char TYPE[5]       = "type";
static char SUBTYPE[8]    = "subtype";
static char ACTION[8]     = "actions";
static SN_SOURCE_RECORD *first_sn_source_record_in_list = NULL;

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

void generate_config_files(char* instance) {
	if (instance == NULL)
		return;

	SN_SOURCE_RECORD* tmp = NULL;
	tmp = initialize_sn_source_record();

	//if (FIRST_DATA_SOURCE == NULL) {
	//FIRST_DATA_SOURCE = pull_sources_from_instance(instance);
	//}
	if (ensure_dir_exists("landing", "") == -1) {
		// implement error here
		return;
	}

	tmp = pull_sources_from_instance(instance);
	while(tmp) {
		printf("\n-----------\nSys ID: %s", tmp->sys_id);
		printf("\nTable: %s", tmp->table);
		printf("\nRecord: %s", tmp->record);
		printf("\nFile Name: %s", tmp->file_name);
		printf("\nType: %s", tmp->type);
		printf("\nSubType: %s\n---------------", tmp->subtype);
		if (ensure_dir_exists(tmp->sys_id, "") == -1) {
			// implement error here
			break;
		}
		if (ensure_dir_exists(tmp->sys_id, "success") == -1) {
			// implement error here
			break;
		}
		if (ensure_file_exists(tmp->sys_id, "config.txt") == -1) {
			//implement error here
			break;
		}

		tmp = tmp->next;
	}
}

SN_SOURCE_RECORD* pull_sources_from_instance(char* instance) {
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

void parse_returned_sn_source_records(char* sources) {
	printf("Got from instance: %s\n", sources);

	JSON_OBJECT *looper = NULL;

	JSON_OBJECT *test = parse_json_from_string(sources);
	if (test) {
		printf("\nFirst JSON Level Name: %s\n", test->name);
		printf("\nFirst JSON level Value: %s\n", (char *)test->data);
		
		if (test->children) {
			looper = initialize_json_object();
			looper = test->children;
			while(looper) {
				printf("\nHad Child JSON Name: %s\n", looper->name);
				printf("\nHad Child JSON Value: %s\n", (char *)looper->data);
				looper = looper->next;
			}
		}
	}

	return;

	char *current_read_string = NULL, *current_key = NULL, *value = NULL, *tmp = NULL;
	int reading = 0, key_hit = -1;
	SN_SOURCE_RECORD *first = NULL, *current_sn_source_record = NULL, *placeholder = NULL;
	current_read_string = malloc(8);
	value = malloc(8);
	register int index = 0;
	register int x = 0;
	if (sources == NULL) {
		return;
	}

	while(sources[index] != '\0') {
		tmp = realloc(current_read_string, (index+1)*8);
		if (tmp == NULL) {
			//implement error check here
			return;
		}
		current_read_string = tmp;
		current_read_string[index] = sources[index];
		key_hit = -1;
		if (reading == 0) {
			key_hit = seek_keys(substring(sources, index-13, index));
			if (key_hit >= 0)
				reading = 1;
		}

		if (reading == 1) {
			if (key_hit >= 0) {
				if (placeholder == NULL)
					placeholder = initialize_sn_source_record();

				if (key_hit == 0) {
					x = 0;
					value = NULL;
					index = index-4;
					while(sources[index] != '\0' && x < 32) {
						tmp = realloc(value, (sizeof(char)*index) + 4);
						if (tmp == NULL) {
							//implement error check here
							return;
						}

						value = tmp;
						value[x] = sources[index];
						printf("\n%s", value);
						x++;
						index++;
					}
					if (value != NULL)
						set_sys_sn_source_record(&placeholder, value);

					reading = 0;
				}
				else if (key_hit == 1) {
					if (strncmp(substring(sources, index-3, index-1), KEY_VALUE_SEPERATOR, 3) == 0) {
						x = 0;
						value = NULL;
						while(sources[index] != '\0'){
							//if (sources[index] == '\"' || sources[index] == '\\' || sources[index] == ':') {
							//	index++;
							//	continue;
							//}
							tmp = realloc(value, (sizeof(char)*index) + 4);
							if (tmp == NULL) {
								//implement error check here
								return;
							}

							value = tmp;
							value[x] = sources[index];
							printf("\n%s", value);
							x++;
							index++;
							if ((strncmp(substring(sources, index, index+3), "\",\"", 3) == 0) ||
									(strncmp(substring(sources, index, index+2), "\"}", 2) == 0)) {

								if (value != NULL) 
									set_name_sn_source_record(&placeholder, value);

								reading = 0;
								break;
							}
						}
					}
				}
				else if (key_hit == 2) {
					if (strncmp(substring(sources, index-3, index-1), KEY_VALUE_SEPERATOR, 3) == 0) {
						x = 0;
						value = NULL;
						while(sources[index] != '\0'){
							//if (sources[index] == '\"' || sources[index] == '\\' || sources[index] == ':') {
							//	index++;
							//	continue;
							//}
							tmp = realloc(value, (sizeof(char)*index) + 4);
							if (tmp == NULL) {
								//implement error check here
								return;
							}

							value = tmp;
							value[x] = sources[index];
							printf("\n%s", value);
							x++;
							index++;
							if ((strncmp(substring(sources, index, index+3), "\",\"", 3) == 0) ||
									(strncmp(substring(sources, index, index+2), "\"}", 2) == 0)) {

								if (value != NULL)
									set_table_sn_source_record(&placeholder, value);

								reading = 0;
								break;
							}
						}
					}
				}
				else if (key_hit == 3) {
					if (strncmp(substring(sources, index-3, index-1), KEY_VALUE_SEPERATOR, 3) == 0) {
						x = 0;
						value = NULL;
						while(sources[index] != '\0'){
							//if (sources[index] == '\"' || sources[index] == '\\' || sources[index] == ':') {
							//	index++;
							//	continue;
							//}
							tmp = realloc(value, (sizeof(char)*index) + 4);
							if (tmp == NULL) {
								//implement error check here
								return;
							}

							value = tmp;
							value[x] = sources[index];
							printf("\n%s", value);
							x++;
							index++;
							if ((strncmp(substring(sources, index, index+3), "\",\"", 3) == 0) ||
									(strncmp(substring(sources, index, index+2), "\"}", 2) == 0)) {

								if (value != NULL)
									set_record_sn_source_record(&placeholder, value);

								reading = 0;
								break;
							}
						}
					}
				}
				else if (key_hit == 4) {
					if (strncmp(substring(sources, index-3, index-1), KEY_VALUE_SEPERATOR, 3) == 0) {
						x = 0;
						value = NULL;
						while(sources[index] != '\0'){
							//if (sources[index] == '\"' || sources[index] == '\\' || sources[index] == ':') {
							//	index++;
							//	continue;
							//}
							tmp = realloc(value, (sizeof(char)*index) + 4);
							if (tmp == NULL) {
								//implement error check here
								return;
							}

							value = tmp;
							value[x] = sources[index];
							printf("\n%s", value);
							x++;
							index++;
							if ((strncmp(substring(sources, index, index+3), "\",\"", 3) == 0) ||
									(strncmp(substring(sources, index, index+2), "\"}", 2) == 0)) {

								if (value != NULL)
									set_type_sn_source_record(&placeholder, value);

								reading = 0;
								break;
							}
						}
					}
				}
				else if (key_hit == 5) {
					if (strncmp(substring(sources, index-3, index-1), KEY_VALUE_SEPERATOR, 3) == 0) {
						x = 0;
						value = NULL;
						while(sources[index] != '\0'){
							//if (sources[index] == '\"' || sources[index] == '\\' || sources[index] == ':') {
							//	index++;
							//	continue;
							//}
							tmp = realloc(value, (sizeof(char)*index) + 4);
							if (tmp == NULL) {
								//implement error check here
								return;
							}

							value = tmp;
							value[x] = sources[index];
							printf("\n%s", value);
							x++;
							index++;
							if ((strncmp(substring(sources, index, index+3), "\",\"", 3) == 0) ||
									(strncmp(substring(sources, index, index+2), "\"}", 2) == 0)) {

								if (value != NULL)
									set_subtype_sn_source_record(&placeholder, value);

								reading = 0;
								break;
							}
						}
					}
				}

				if (placeholder != NULL && placeholder->sys_id != NULL && placeholder->file_name != NULL) {
					if (first == NULL) {
						first = initialize_sn_source_record();
						first = placeholder;
						placeholder = NULL;
						free(placeholder);
						current_sn_source_record = initialize_sn_source_record();
						current_sn_source_record = first;
						reading = 0;
					}
					else {
						current_sn_source_record->next = initialize_sn_source_record();
						current_sn_source_record->next = placeholder;
						placeholder = NULL;
						free(placeholder);
						current_sn_source_record = current_sn_source_record->next;
						reading = 0;
					}
				}
			}
		}
		index++;
	}
	if (first) {
		first_sn_source_record_in_list = initialize_sn_source_record();
		first_sn_source_record_in_list = first;
	}

	return;
}

void set_sys_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *sys) {
	(*sn_source_record)->sys_id = malloc(33);
	if ((*sn_source_record)->sys_id == NULL) {
		// Implement Error functionality
		return;
	}

	(*sn_source_record)->sys_id = sys;
}

void set_name_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *name) {
	(*sn_source_record)->file_name = malloc(sizeof(char) * strlen(name) + 4);
	if ((*sn_source_record)->file_name == NULL) {
		// Implement Error functionality
		return;
	}

	(*sn_source_record)->file_name = name;
}

void set_table_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *table) {
	(*sn_source_record)->table = malloc(sizeof(char) * strlen(table) + 4);
	if ((*sn_source_record)->table == NULL) {
		// Implement Error functionality
		return;
	}

	(*sn_source_record)->table = table;
}

void set_record_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *record) {
	(*sn_source_record)->record = malloc(sizeof(char) * strlen(record) + 4);
	if ((*sn_source_record)->record == NULL) {
		// Implement Error functionality
		return;
	}

	(*sn_source_record)->record = record;
}

void set_type_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *type) {
	(*sn_source_record)->type = malloc(sizeof(char) * strlen(type) + 4);
	if ((*sn_source_record)->type == NULL) {
		// Implement Error functionality
		return;
	}

	(*sn_source_record)->type = type;
}

void set_subtype_sn_source_record(SN_SOURCE_RECORD **sn_source_record, char *subtype) {
	(*sn_source_record)->subtype = malloc(sizeof(char) * strlen(subtype) + 4);
	if ((*sn_source_record)->subtype == NULL) {
		// Implement Error functionality
		return;
	}

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
	(*ps)->file_name = malloc(sizeof(char) * strlen(name) + 4);
	if ((*ps)->file_name == NULL) {
		// Implement Error functionality
		return;
	}

	(*ps)->file_name = name;
}

void set_prepend_file(PUSH_CONFIG **ps, char *name) {
	(*ps)->file_to_prepend = malloc(sizeof(char) * strlen(name) + 4);
	if ((*ps)->file_to_prepend == NULL) {
		// Implement Error functionality
		return;
	}

	(*ps)->file_to_prepend = name;
}

void set_append_file(PUSH_CONFIG **ps, char *name) {
	(*ps)->file_to_append = malloc(sizeof(char) * strlen(name) + 4);
	if ((*ps)->file_to_append == NULL) {
		// Implement Error functionality
		return;
	}

	(*ps)->file_to_append = name;
}
void set_file_filter(PUSH_CONFIG **ps, char *filter) {
	(*ps)->file_name_filter = malloc(sizeof(char) * strlen(filter) + 4);
	if ((*ps)->file_name_filter == NULL) {
		// Implement Error functionality
		return;
	}

	(*ps)->file_name_filter = filter;
}
