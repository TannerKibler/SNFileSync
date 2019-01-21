#include "file.h"

#ifdef WINDOWS
static char buffer[MAX_PATH];
#else
struct stat st = {0};
#endif


char* get_current_directory() {
	int toTrim = 0;
	char* tmp = NULL;
	tmp = malloc(MAX_PATH);
#ifdef WINDOWS
	GetModuleFileName( NULL, buffer, MAX_PATH );
	strcpy(tmp, buffer);
	toTrim = backwards_find_index(tmp, '\\'); 
	if (toTrim != -1) {
		tmp = (char*)substring(tmp, 0, toTrim);
	}
	//return *buffer;
	return tmp;
#else
	if (getcwd(tmp, MAX_PATH) != NULL) {
		return tmp;
	} else {
		perror("getcwd() error");
		return NULL;
	}
	return NULL;
#endif
}

int ensure_dir_exists(char* l1, char* l2) {
	char* dir = malloc(MAX_PATH + (sizeof(char)*strlen(l1)) + (sizeof(char)*strlen(l2)) + 8);
	dir = get_current_directory();
#ifdef WINDOWS
	strcat(dir, l1);
	if (l2 != NULL) {
		strcat(dir, "\\");
		strcat(dir, l2);
	}
	printf("Current directory: %s\n", dir);
	if (CreateDirectory(dir, NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		return 1;
	}
	else {
		return -1;
	}
	return 0;
#else
	strcat(dir, "/");
	strcat(dir, l1);
	if (l2 != NULL) {
		strcat(dir, "/");
		strcat(dir, l2);
	}
	printf("Current directory: %s\n", dir);
	// Failed to create directory.
	DIR* directory = opendir(dir);
	if (directory) {
		closedir(directory);
		return 1;
	}
	else if (ENOENT == errno)
		return mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	else {
		/* opendir() failed for some other reason. */

		// implement some error here
		return -1;
	}

	return 0;
#endif
}

int ensure_file_exists(char* l1, char* l2) {
	char* dir = malloc(sizeof(char)*strlen(l1) + (sizeof(char)*strlen(l2)) + 8);
	if (dir == NULL) {
		//implement failure
		return -1;
	}
	dir = get_current_directory();

#ifdef WINDOWS
	strcat(dir, l1);
	if (l2 != NULL) {
		strcat(dir, "\\");
		strcat(dir, l2);
	}

	printf("Checking for file: %s\n", dir);
#else
	strcat(dir, "/");
	strcat(dir, l1);
	if (l2 != NULL) {
		strcat(dir, "/");
		strcat(dir, l2);
	}
	
	printf("Checking for file: %s\n", dir);
#endif

	FILE *fp = fopen(dir, "ab+");
	if (fp)
		return 1;
	else
		return -1;

	return 0;
}

char* read_file_to_buffer(char* fileName) {
	char *buffer = 0;
	long length;

	//TODO: Add Error checking
	FILE *f = fopen (fileName, "rb");
	if (f) {
		fseek (f, 0, SEEK_END);
		length = ftell (f);
		fseek (f, 0, SEEK_SET);
		buffer = malloc (length + 1);
		if (buffer) {
			fread (buffer, 1, length, f);
		}
		fclose (f);
		buffer[length] = '\0';
	}

	return buffer;
}
