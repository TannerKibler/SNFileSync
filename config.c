#include "config.h"

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
	printf("Received: %s\n", instance);
	if (instance == NULL)
		return;

	DATA_SOURCE* tmp = NULL;
	tmp = initialize_data_source();

	//if (FIRST_DATA_SOURCE == NULL) {
		//FIRST_DATA_SOURCE = pull_sources_from_instance(instance);
	//}

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
	load_data_sources(instance); //DEFAULTING THIS UNTIL I FIGURE OUT CURL LINKING ISSUES
	DATA_SOURCE *ds = NULL;
	ds = initialize_data_source();

	set_sys_data_source(&ds, "643b49f7133e5f0022785d622244b02c");
	set_name_data_source(&ds, "Asset Data Load");

	printf("Sys id: %s\n", ds->sys_id);
	printf("Name: %s\n", ds->name);

	return ds;
}

DATA_SOURCE* initialize_data_source() {
	DATA_SOURCE *ds = NULL;
	ds = malloc((sizeof(DATA_SOURCE)) + 8);
	if (ds == NULL) {
		// Implement error functionality
		return NULL;
	}

	ds->next = NULL;

	return ds;
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