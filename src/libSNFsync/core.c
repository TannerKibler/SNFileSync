#include "core.h"

static pthread_t tid[2];

void set_string_value(char* strSource, char** strDest) {
	printf("Setting strDest to : %s\n", strSource);
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
	printf("\n\nStarting on: %i", start_index);

	do {
		start_index++;
		if (search_string[start_index] == to_find) {
			printf("\nEnding on: %i\n", start_index);
			return start_index;
		}
	}
	while(search_string[start_index+1] != '\0');

	return -1;
}

void* poll_changes_to_landing_directory(void *thread_data) {
	printf("\nStarted watching landing directory\n");
	for (;;) {
		sleep(5);
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

	err = pthread_create(&(tid[0]), NULL, &poll_changes_to_landing_directory, NULL);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
	else
		printf("\n Thread created successfully\n");

	while(1) {
		if (watch_for_file_system_changes() == 0) {
			printf("\nDetected file change\n");
		}
		else {
			//implement error library
			printf("\nFailed to initialize inotify\n");
			break;
		}
	}
}

int watch_for_file_system_changes() {
	static int inotify_init_ret = 0, inotify_read = 0, i = 0, set_return_value = -1;
	//static char *cwd = NULL;
	static char buffer[INOTIFY_BUFFER_LENGTH], file_buffer[MAX_PATH];
	struct inotify_event *event = NULL;

	inotify_init_ret = inotify_init();
	if (inotify_init_ret < 0) {
		//implement error library
		return -1;
	}

	get_current_directory(file_buffer);
	//if (!file_buffer) {
	//	//implement error library
	//	printf("\nFailed to get current directory\n");
	//	return -1;
	//}

	printf("\nCurrent Working Directory is : %s\n", file_buffer);

	if (ensure_dir_exists(file_buffer, "landing", NULL) == -1) {
		//implement error library
		printf("\nFailed to create / find landing directory\n");
		return -1;
	}

	inotify_add_watch(inotify_init_ret, "/home/arch/code/c/SNFileSync/misc/landing", IN_CREATE | IN_MODIFY);
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
