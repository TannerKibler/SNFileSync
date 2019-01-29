#include "file.h"

#ifdef WINDOWS
static char buffer[MAX_PATH];
#else
struct stat st = {0};
#endif


char* get_current_directory(char *buffer) {
	//char* tmp = NULL;
	//tmp = malloc(MAX_PATH);
#ifdef WINDOWS
	int toTrim = 0;
	GetModuleFileName( NULL, buffer, MAX_PATH );
	//strcpy(tmp, buffer);
	toTrim = backwards_find_index(buffer, '\\'); 
	if (toTrim != -1) {
		buffer = (char*)substring(tmp, 0, toTrim);
	}
	//return *buffer;
	return tmp;
#else
	if (getcwd(buffer, MAX_PATH) != NULL) {
		return buffer;
	} else {
		perror("getcwd() error");
		return NULL;
	}
	return NULL;
#endif
}

int ensure_dir_exists(char *buffer, char* l1, char* l2) {
	get_current_directory(buffer);
#ifdef WINDOWS
	strcat(buffer, l1);
	if (l2 != NULL) {
		strcat(buffer, "\\");
		strcat(buffer, l2);
	}
	printf("Current directory: %s\n", buffer);
	if (CreateDirectory(buffer, NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		return 1;
	}
	else {
		return -1;
	}
	
	return 0;
#else
	strcat(buffer, "/");
	strcat(buffer, l1);
	if (l2 != NULL) {
		strcat(buffer, "/");
		strcat(buffer, l2);
	}
	printf("Current directory: %s\n", buffer);
	// Failed to create directory.
	DIR* directory = opendir(buffer);
	if (directory) {
		closedir(directory);
		return 1;
	}
	else if (ENOENT == errno) {
		if (mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
			return 1;
	}
	else {
		/* opendir() failed for some other reason. */
		return -1;
	}

	return 0;
#endif
}

void generate_config_file_from_source_record(SN_SOURCE_RECORD *to_store, char *path, char *file_name) {
	//char* dir = malloc(sizeof(char)*strlen(path) + (sizeof(char)*strlen(file_name)) + 8);
	//if (dir == NULL) {
	//	//implement failure
	//	return;
	//}
	char buffer[MAX_PATH];
	get_current_directory(buffer);

#ifdef WINDOWS
	strcat(buffer, path);
	if (file_name != NULL) {
		strcat(buffer, "\\");
		strcat(buffer, file_name);
	}
	else
		return;

	printf("Checking for file: %s\n", buffer);
#else
	strcat(buffer, "/");
	strcat(buffer, path);
	if (file_name != NULL) {
		strcat(buffer, "/");
		strcat(buffer, file_name);
	}
	else
		return;

	printf("Checking for file: %s\n", buffer);
#endif

	printf("Checking for file: %s\n", buffer);

	FILE *file_to_write = fopen(buffer, "w");
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
	//free(buffer);
}

void generate_file_for_instance_config(SN_INSTANCE *instance, char *path, char *file_name) {
	//char* dir = malloc(sizeof(char)*strlen(path) + (sizeof(char)*strlen(file_name)) + 8);
	//if (dir == NULL) {
	//	//implement failure
	//	return;
	//}
	char buffer[MAX_PATH];
	get_current_directory(buffer);

#ifdef WINDOWS
	strcat(buffer, path);
	if (file_name != NULL) {
		strcat(buffer, "\\");
		strcat(buffer, file_name);
	}
	else
		return;

	printf("Checking for file: %s\n", buffer);
#else
	strcat(buffer, "/");
	strcat(buffer, path);
	if (file_name != NULL) {
		strcat(buffer, "/");
		strcat(buffer, file_name);
	}
	else
		return;

	printf("Checking for file: %s\n", buffer);
#endif

	printf("Checking for file: %s\n", buffer);

	FILE *file_to_write = fopen(buffer, "w");
	if (file_to_write == NULL) {
		//implement error library
		return;
	}

	fprintf(file_to_write, "username:%s\n", instance->username);
	fprintf(file_to_write, "password:%s\n", instance->password);
	fprintf(file_to_write, "instance:%s\n", instance->host_name);

	fclose(file_to_write);
	//free(dir);
}

int ensure_file_exists(char *buffer, char* l1, char* l2) {
//	char* dir = malloc(sizeof(char)*strlen(l1) + (sizeof(char)*strlen(l2)) + 8);
//	if (dir == NULL) {
//		//implement failure
//		return -1;
//	}
//	dir = get_current_directory();
	get_current_directory(buffer);

#ifdef WINDOWS
	strcat(buffer, l1);
	if (l2 != NULL) {
		strcat(buffer, "\\");
		strcat(buffer, l2);
	}

	printf("Checking for file: %s\n", buffer);
#else
	strcat(buffer, "/");
	strcat(buffer, l1);
	if (l2 != NULL) {
		strcat(buffer, "/");
		strcat(buffer, l2);
	}

	printf("Checking for file: %s\n", buffer);
#endif

	FILE *fp = fopen(buffer, "ab+");
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
