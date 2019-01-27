#include "file.h"

#ifdef WINDOWS
static char buffer[MAX_PATH];
#else
struct stat st = {0};
#endif


char* get_current_directory() {
	char* tmp = NULL;
	tmp = malloc(MAX_PATH);
#ifdef WINDOWS
	int toTrim = 0;
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

void generate_config_file_from_source_record(SN_SOURCE_RECORD *to_store, char *path, char *file_name) {
	char* dir = malloc(sizeof(char)*strlen(path) + (sizeof(char)*strlen(file_name)) + 8);
	if (dir == NULL) {
		//implement failure
		return;
	}
	dir = get_current_directory();

#ifdef WINDOWS
	strcat(dir, path);
	if (file_name != NULL) {
		strcat(dir, "\\");
		strcat(dir, file_name);
	}
	else
		return;

	printf("Checking for file: %s\n", dir);
#else
	strcat(dir, "/");
	strcat(dir, path);
	if (file_name != NULL) {
		strcat(dir, "/");
		strcat(dir, file_name);
	}
	else
		return;
	
	printf("Checking for file: %s\n", dir);
#endif
	
	printf("Checking for file: %s\n", dir);
	
	FILE *file_to_write = fopen(dir, "w");
	if (file_to_write == NULL) {
		//implement error library
		return;
	}

	fprintf(file_to_write, "sys_id:%s\n", to_store->sys_id);
	fprintf(file_to_write, "table:%s\n", to_store->table);
	fprintf(file_to_write, "record:%s\n", to_store->record);
	fprintf(file_to_write, "type:%s\n", to_store->type);

	if (to_store->subtype != NULL)
		fprintf(file_to_write, "subtype:%s\n", to_store->subtype);

	fprintf(file_to_write, "file_name:%s\n", to_store->file_name);

	fclose(file_to_write);
	free(dir);
}

void generate_file_for_instance_config(SN_INSTANCE *instance, char *path, char *file_name) {
	char* dir = malloc(sizeof(char)*strlen(path) + (sizeof(char)*strlen(file_name)) + 8);
	if (dir == NULL) {
		//implement failure
		return;
	}
	dir = get_current_directory();

#ifdef WINDOWS
	strcat(dir, path);
	if (file_name != NULL) {
		strcat(dir, "\\");
		strcat(dir, file_name);
	}
	else
		return;

	printf("Checking for file: %s\n", dir);
#else
	strcat(dir, "/");
	strcat(dir, path);
	if (file_name != NULL) {
		strcat(dir, "/");
		strcat(dir, file_name);
	}
	else
		return;
	
	printf("Checking for file: %s\n", dir);
#endif
	
	printf("Checking for file: %s\n", dir);
	
	FILE *file_to_write = fopen(dir, "w");
	if (file_to_write == NULL) {
		//implement error library
		return;
	}

	fprintf(file_to_write, "username:%s\n", instance->username);
	fprintf(file_to_write, "password:%s\n", instance->password);
	fprintf(file_to_write, "instance::%s\n", instance->host_name);

	fclose(file_to_write);
	free(dir);
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
