#include "config.h"

static char SYS_ID[7] = "sys_id\0";
static char NAME[5] = "name\0";
static DATA_SOURCE *first_ds_in_list = NULL;

void seek_keys(char *str, char **retStr) {
	char *key = NULL;

	if (str == NULL) {
		return;
	}

	if (strncmp(str, SYS_ID, 6) == 0) {
		if (*retStr == NULL) {
			*retStr = malloc(sizeof(char) * 10);
			if (*retStr != NULL) {
				strcpy(*retStr, SYS_ID);
			}
		}
		else {
			key = realloc(*retStr, sizeof(char) * 10);
			if (key == NULL) {
				return;
			}
			*retStr = key;
			strcpy(*retStr, SYS_ID);

		}
		return;
	}
	else if (strncmp(str, NAME, 4) == 0) {
		if (*retStr == NULL) {
			*retStr = malloc(sizeof(char) * 10);
			if (*retStr != NULL) {
				strcpy(*retStr, NAME);
			}
		}
		else {
			key = realloc(*retStr, sizeof(char) * 10);
			if (key == NULL) {
				return;
			}
			*retStr = key;
			strcpy(*retStr, NAME);

		}
		return;
	}

	return;
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

	DATA_SOURCE* tmp = NULL;
	tmp = initialize_data_source();

	//if (FIRST_DATA_SOURCE == NULL) {
	//FIRST_DATA_SOURCE = pull_sources_from_instance(instance);
	//}
	if (ensure_dir_exists("landing", "") == -1) {
		// implement error here
		return;
	}

	tmp = pull_sources_from_instance(instance);
	while(tmp) {
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

DATA_SOURCE* pull_sources_from_instance(char* instance) {
	DATA_SOURCE *ds = NULL;
	load_data_sources(instance); //DEFAULTING THIS UNTIL I FIGURE OUT CURL LINKING ISSUES
	if (first_ds_in_list == NULL)
		return NULL;

	return first_ds_in_list;
}

DATA_SOURCE* initialize_data_source() {
	DATA_SOURCE *ds = NULL;
	ds = malloc((sizeof(DATA_SOURCE)) + 8);
	if (ds == NULL) {
		// Implement error functionality
		return NULL;
	}

	ds->sys_id = NULL;
	ds->name   = NULL;
	ds->next   = NULL;

	return ds;
}

void parse_returned_data_sources(char* sources) {
	printf("Got from instance: %s\n", sources);

	char curr_char;
	char *current_read_string = NULL, *current_key = NULL, *value = NULL, *tmp = NULL;
	char *sys_id = NULL, *name = NULL;
	int reading = 0;
	DATA_SOURCE *first = NULL, *current_ds = NULL, *placeholder = NULL;
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
		if (reading == 0) {
			if (index > 5) { // long enough to potentially contain my keys
				seek_keys(substring(sources, index-6, index), &current_key);
				if (current_key == NULL)
					seek_keys(substring(sources, index-5, index), &current_key);
			}
			if (current_key)
				reading = 1;
		}

		if (reading == 1) {
			if (placeholder == NULL)
				placeholder = initialize_data_source();
			if (strncmp(current_key, SYS_ID, 6) == 0) { // reading sys
				if (strncmp(substring(sources, index-3, index-1), KEY_VALUE_SEPERATOR, 3) == 0) {
					x = 0;
					value = NULL;
					while(sources[index] != '\0' && x < 32) {
						tmp = realloc(value, (index+1)*8);
						if (tmp == NULL) {
							//implement error check here
							return;
						}

						value = tmp;
						value[x] = sources[index];
						x++;
						index++;
					}
					if (value != NULL)
						set_sys_data_source(&placeholder, value);

					reading = 0;
					current_key = NULL;
				}
			}
			else if (strncmp(current_key, NAME, 4) == 0) { // reading name
				if (strncmp(substring(sources, index-3, index-1), KEY_VALUE_SEPERATOR, 3) == 0) {
					x = 0;
					value = NULL;
					while(sources[index] != '\0'){
						tmp = realloc(value, (index+1)*8);
						if (tmp == NULL) {
							//implement error check here
							return;
						}

						value = tmp;
						value[x] = sources[index];
						x++;
						index++;
						if ((strncmp(substring(sources, index, index+3), "\",\"", 3) == 0) ||
								(strncmp(substring(sources, index, index+2), "\"}", 2) == 0)) {
							if (value != NULL) {
								set_name_data_source(&placeholder, value);
								break;
							}

							reading = 0;
							current_key = NULL;
						}
					}
				}
			}
			if (placeholder != NULL && placeholder->sys_id != NULL && placeholder->name != NULL) {
				if (first == NULL) {
					first = initialize_data_source();
					first = placeholder;
					placeholder = NULL;
					free(placeholder);
					current_ds = initialize_data_source();
					current_ds = first;
					reading = 0;
					current_key = NULL;
				}
				else {
					current_ds->next = initialize_data_source();
					current_ds->next = placeholder;
					placeholder = NULL;
					free(placeholder);
					current_ds = current_ds->next;
					reading = 0;
					current_key = NULL;
				}
			}
		}
		index++;
	}
	if (first) {
		first_ds_in_list = initialize_data_source();
		first_ds_in_list = first;
	}

	return;
}

void set_sys_data_source(DATA_SOURCE **ds, char *sys) {
	(*ds)->sys_id = malloc(33);
	if ((*ds)->sys_id == NULL) {
		// Implement Error functionality
		return;
	}

	(*ds)->sys_id = sys;
	//(*ds)->sys_id[33] = '\0';
}

void set_name_data_source(DATA_SOURCE **ds, char *name) {
	(*ds)->name = malloc(sizeof(char) * strlen(name) + 4);
	if ((*ds)->name == NULL) {
		// Implement Error functionality
		return;
	}

	(*ds)->name = name;
	//(*ds)->name[strlen(name)-1] = '\0';
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
