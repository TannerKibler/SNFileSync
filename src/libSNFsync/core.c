#include "core.h"

static pthread_t tid[2];

void set_string_value(char* strSource, char** strDest) {
	if (strSource == NULL)
		return;

	*strDest = malloc(sizeof(char)*strlen(strSource) + 4);
	if (*strDest == NULL) {
		// error allocating
		// handle this
		return;
	}

	strcpy(*strDest, strSource);
}

char* substring(char *str, int stIndex, int enIndex) {
	char *retString = NULL;
	int loopIn = 0;

	if (enIndex < stIndex)
		return NULL;

	if ((enIndex - stIndex) > 1)
		retString = malloc((sizeof(char)*(enIndex - stIndex) + 8));
	else
		retString = malloc(32);
	while(stIndex <= enIndex && str[stIndex] != '\0') {
		retString[loopIn] = str[stIndex];
		stIndex++;
		loopIn++;
	}

	retString[loopIn] = '\0';

	return retString;
}

int backwards_find_index_from_index(char* str, char toFind, int start) {
	if (str == NULL)
		return -1;

	for (; start >= 0; start--) {
		if (str[start] == toFind)
			return start;
	}

	return -1;
}

int backwards_find_index(char* str, char toFind) {
	if (str == NULL)
		return -1;

	int last = strlen(str)-1;
	for (; last >= 0; last--) {
		if (str[last] == toFind)
			return last;
	}

	return -1;
}

int find_index_of_next_occurence(char to_find, int start_index, char *search_string) {
	if (!to_find || search_string == NULL || !search_string[start_index] || search_string[start_index] == '\0') {
		// implement error library
		return -1;
	}

	do {
		start_index++;
		if (search_string[start_index] == to_find) {
			return start_index;
		}
	}
	while(search_string[start_index+1] != '\0');

	return -1;
}

void* poll_changes_to_landing_directory(void *thread_data) {
	while (1) {
		sleep(5);
		process_new_files();
		printf("\nStayed live for another loop.\n");
	}
	return NULL;
}

void start_watching_landing_directory_for_instance() {
	// This should run until forced to stop
	int err = 0;
	int daemon_success = 0;

	daemon_success = create_daemon();
	if (daemon_success != 0) {
		//implement error library
		return;
	}

	process_new_files();

	//err = pthread_create(&(tid[0]), NULL, &poll_changes_to_landing_directory, NULL);
	//if (err != 0) {
	//	//implement error library
	//	return;
	//}

	while(1) {
		if (watch_for_file_system_changes() == 0) {
			process_new_files();
		}
	}
	
}

void* loop_for_file_system_watch(void *thread_data) {
	while(1) {
		if (watch_for_file_system_changes() == 0) {
			printf("\nDetected file change\n");
			process_new_files();
		}
	}
}

void process_new_files() {
	char** files = read_files_in_landing_directory();
	SN_SOURCE_RECORD *first_in_list = NULL, *looper = NULL;
	register int i = 0;
	int file_name_match = 0;

	first_in_list = get_first_sn_source_record();
	looper = first_in_list;
	if (!first_in_list) {
		//implement error library
		printf("\nFailed to get first in list: source records\n");
		return;
	}

	while(files[i]) {
		if (!files[i] || (strncmp(files[i], ".", 1) == 0) || (strncmp(files[i], "..", 2) == 0)) {
			i++;
			continue;
		}
		printf("\nProcessing file: %s\n", files[i]);

		while(looper) {
			file_name_match = check_file_name_for_match(files[i], looper);
			if (file_name_match > 0) {
				handle_file_by_source_record_rules(files[i], looper);
				break;
			}

			looper = looper->next;
		}

		looper = first_in_list;
		i++;
	}
}

void handle_file_by_source_record_rules(char *file_name, SN_SOURCE_RECORD *sn_source_record) {
	printf("\nWow, can't believe that worked\n");
}

int check_file_name_for_match(char *file_name, SN_SOURCE_RECORD *sn_source_record) {
	if (!file_name || !sn_source_record || !sn_source_record->file_name)
		return -1;

	register int i = 0, y = 0;
	int return_value = 0, wildcard_asterisk = 0, wildcard_percent = 0, index_1 = 0, index_2 = 0;
	int file_name_length = 0;
	char *substr_comp_1 = NULL, *substr_comp_2 = NULL;

	file_name_length = strlen(file_name);

	printf("\n----------\nChecking file name for %s\n----------\n", file_name);

	while(i < file_name_length) {
		if (sn_source_record->file_name[i] == '*') {
			if (return_value != 0 && (sn_source_record->file_name[i+1] == '\0' || !sn_source_record->file_name[i+1])) {
				// we've hit a trailing asterisk and we have had a match so far, return true
				return return_value;
			}
			else if (return_value == 0 && (sn_source_record->file_name[i+1] == '\0' || !sn_source_record->file_name[i+1])) {
				// we've hit a trailing asterisk and we have NOT had a match so far
				if (i == 0) {
					// my only character is an asterisk, this should always be true
					return 1;
				}

				// I'm not the first character, and so far I've failed. return false
				return 0;
			}
			else {
				// I'm not at the end, but I am an asterisk
				if (i == 0) {
					if (file_name_length == 1) {
						return 0;
					}
					else {
						return_value = 1;
						wildcard_asterisk = 1;
						i++;
						continue;
					}
				}
			}
		}
		else if (sn_source_record->file_name[i] == '%') {
			if (return_value != 0 && (sn_source_record->file_name[i+1] == '\0' || !sn_source_record->file_name[i+1])) {
				// we've hit a trailing percent and we have had a match so far, return true
				return return_value;
			}
			else if (return_value == 0 && (sn_source_record->file_name[i+1] == '\0' || !sn_source_record->file_name[i+1])) {
				// we've hit a trailing percent and we have NOT had a match so far
				if (i == 0) {
					// my only character is an percent, this should always be true
					return 1;
				}

				// I'm not the first character, and so far I've failed. return false
				return 0;
			}
			else {
				// I'm not at the end, but I am an percent
				if (i == 0) {
					if (file_name_length == 1) {
						return 0;
					}
					else {
						return_value = 1;
						wildcard_percent = 1;
						i++;
						continue;
					}
				}
			}
		}
		else {
			y = i;
			while (y < file_name_length) {
				if (wildcard_asterisk == 1) {
					// proceed until we hit a match again
					if (file_name[y] == sn_source_record->file_name[i]) {
						index_1 = find_index_of_next_occurence(file_name[y], y+1, file_name);
						if (index_1 > -1) { // if I see this character again later, I can't confirm yet that I should stop with wildcard
							substr_comp_1 = malloc(sizeof(char)*5);
							substr_comp_1 = substring(file_name, index_1, index_1 + 4);
							substr_comp_2 = malloc(sizeof(char)*5);
							substr_comp_2 = substring(sn_source_record->file_name, i+1, i+5);

							if (strncmp(substr_comp_1, substr_comp_2, 5) == 0) { // if the next occurrence of the char includes subsequent matches, skip to there
								i = index_1;
								wildcard_asterisk = 0;
								return_value = 1;
								free(substr_comp_1);
								free(substr_comp_2);
								break;
							}
						}
						else { // if I do NOT see this character again, assume I've cleared the wildcard
							if (y+1 >= file_name_length) {
								if (sn_source_record->file_name[y+1] != '\0' || !sn_source_record->file_name[y+1]) {
									// file is out of characters but the comparator continues. force failure
									if (sn_source_record->file_name[y+1] == '%' || sn_source_record->file_name[y+1] == '*') {
										return 1;
									}
									return 0;
								}
							}
							i = y;
							wildcard_asterisk = 0;
							return_value = 1;
							break;
						}
					}
					else {
						y++;
						if (y >= file_name_length) {
							// non trailing asterisk, and the end of strings don't match
							return 0;
						}
					}
				}
				else if (wildcard_percent == 1) {
					if (file_name[y] == sn_source_record->file_name[i]) {
						index_1 = find_index_of_next_occurence(file_name[y], y+1, file_name);
						if (index_1 > -1) { // if I see this character again later, I can't confirm yet that I should stop with wildcard
							substr_comp_1 = malloc(sizeof(char)*5);
							substr_comp_1 = substring(file_name, index_1, index_1 + 4);
							substr_comp_2 = malloc(sizeof(char)*5);
							substr_comp_2 = substring(sn_source_record->file_name, i+1, i+5);

							if (strncmp(substr_comp_1, substr_comp_2, 5) == 0) { // if the next occurrence of the char includes subsequent matches, skip to there
								i = index_1;
								wildcard_asterisk = 0;
								return_value = 1;
								free(substr_comp_1);
								free(substr_comp_2);
								break;
							}
						}
						else { // if I do NOT see this character again, assume I've cleared the wildcard
							i = index_1;
							wildcard_asterisk = 0;
							return_value = 1;
							break;
						}
					}
					else {
						if (y+1 >= file_name_length) {
							if (sn_source_record->file_name[y+1] != '\0' || !sn_source_record->file_name[y+1]) {
								// file is out of characters but the comparator continues. force failure
								if (sn_source_record->file_name[y+1] == '%' || sn_source_record->file_name[y+1] == '*') {
									return 1;
								}
								return 0;
							}
						}
						i = y;
						wildcard_asterisk = 0;
						return_value = 1;
						break;
					}
				}
				else {
					if (y > 0 && return_value == 0)
						return return_value;

					printf("1: %d, 2: %d\n", file_name[y], sn_source_record->file_name[y]);
					if (file_name[y] == sn_source_record->file_name[y]) {
						return_value = 1;
						y++;
					}
					else
						return 0;
				}
			}
		}
		i++;
	}

	return return_value;
}

int watch_for_file_system_changes() {
	static int inotify_init_ret = 0, inotify_read = 0, i = 0, set_return_value = -1;
	static char buffer[INOTIFY_BUFFER_LENGTH], file_buffer[MAX_PATH];
	struct inotify_event *event = NULL;

	inotify_init_ret = inotify_init();
	if (inotify_init_ret < 0) {
		//implement error library
		printf("\nFailed to init inotify\n");
		return -1;
	}

	get_current_directory(file_buffer);

	if (ensure_dir_exists(file_buffer, "landing", NULL) == -1) {
		//implement error library
		printf("\nFailed to create / find landing directory\n");
		return -1;
	}

	inotify_add_watch(inotify_init_ret, file_buffer, IN_CREATE | IN_MODIFY);
	inotify_read = read(inotify_init_ret, buffer, INOTIFY_BUFFER_LENGTH);
	if (inotify_read < 0) {
		// implement error library
		printf("\nInotify Read returned failure\n");
		return -1;
	}

	while (i < inotify_read) {
		event = (struct inotify_event *)&buffer[i];
		if (event->len) {
			if (event->mask & IN_MODIFY || event->mask & IN_CREATE) {
				set_return_value = 0;
				printf("\nHad FS event\n");
				//break;
			}
		}
		i += INOTIFY_EVENT_SIZE + event->len;
	}

	return set_return_value;
}

int create_daemon() {
	pid_t forked_pid;

	forked_pid = fork();
	if (forked_pid < 0) {
		//implement error library
		return -1;
	}

	if (setsid() < 0) {
		//implement error library
		return -1;
	}

	return 0;
}
