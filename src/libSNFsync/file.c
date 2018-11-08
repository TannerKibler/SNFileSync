#include "file.h"

#ifdef WINDOWS
static char buffer[MAX_PATH];
#else
struct stat st = {0};
#endif


char* get_current_directory() {
#ifdef WINDOWS
	int toTrim = 0;
	char* tmp = NULL;
	tmp = malloc(MAX_PATH);
	GetModuleFileName( NULL, buffer, MAX_PATH );
	strcpy(tmp, buffer);
	toTrim = backwards_find_index(tmp, '\\'); 
	if (toTrim != -1) {
		tmp = (char*)substring(tmp, 0, toTrim);
	}
	//return *buffer;
	return tmp;
#else
	return NULL;
#endif
}

int ensure_dir_exists(char* l1, char* l2) {
#ifdef WINDOWS
	char* dir = malloc(MAX_PATH + (sizeof(char)*strlen(l1)) + (sizeof(char)*strlen(l2)) + 8);
	dir = get_current_directory();
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
		// Failed to create directory.
		return -1;
	}
	return 0;
#else
	return -1;
#endif
}

int ensure_file_exists(char* l1, char* l2) {
	char* dir = malloc(sizeof(char)*strlen(l1) + (sizeof(char)*strlen(l2)) + 8);
	if (dir == NULL) {
		//implement failure
		return -1;
	}
	dir = get_current_directory();
	strcat(dir, l1);
	if (l2 != NULL) {
		strcat(dir, "\\");
		strcat(dir, l2);
	}

	printf("Checking for file: %s\n", dir);
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
