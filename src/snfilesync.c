#include <stdlib.h>
#include <unistd.h>
#include "libSNFsync/config.h"
#include "libSNFsync/core.h"

int main (int argc, char **argv){
	char *instance = NULL, *pass = NULL, *username = NULL;
	int opt;
	while ((opt = getopt(argc, argv, "glwri:u:")) != -1) {
		switch (opt) {
			case 'i':
				set_string_value(optarg, &instance);
				break;
			case 'u':
				set_string_value(optarg, &username);
				break;
			case 'g':
				printf("Username: ");
				username = malloc(40);
				if (!username) {
					//implement error library
					break;
				}
				fgets(username, 39,stdin);
				username[strcspn(username, "\n")] = 0;
				if (username[0] == '\0') {
					//implement error library
					printf("\nYou forgot to provide your username\n");
					return -1;
				}
				set_string_value(getpass("Password: \0"), &pass);
				if (pass[0] == '\0') {
					//implement error library
					printf("\nYou forgot to provide your password\n");
					return -1;
				}
				generate_config_files(instance, username, pass);
				free(username);
				break;
			case 'w':
				printf("\nForcing check for files\n---------------------\n");
				load_instances_from_files();
				load_sources_from_sn_instance();
				start_watching_landing_directory_for_instance();
				break;
			case 'l':
				load_instances_from_files();
				load_sources_from_sn_instance();
				break;
			case 'r':
				load_instances_from_files();
				//load_sources_from_sn_instance();
				//pass = get_password_for_source_record(get_first_sn_source_record());
				//printf("\nGot Password: %s\n", pass);
		}

		free(pass);
	}
}
