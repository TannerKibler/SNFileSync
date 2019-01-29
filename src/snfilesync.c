#include <stdlib.h>
#include <unistd.h>
#include "libSNFsync/config.h"
#include "libSNFsync/core.h"

int main (int argc, char **argv){
	char *instance = NULL, *pass = NULL, *username = NULL;
	int opt;
	printf("Starting!\n");
	printf("Arguments: %i\n", argc);
	 while ((opt = getopt(argc, argv, "gwi:u:")) != -1) {
        switch (opt) {
        case 'i':
            printf("type: %s\n", optarg);
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
			start_watching_landing_directory_for_instance();
			break;
    }

	free(pass);
}
	//for (x; x < argc; x++) {
		//stmp = argv[x];
	//	printf("Argument: %c\n", (*argv)[x]);
	//	if ((*argv)[x] == 'g')
	//		generate_config_files(instance);
	//	if ((*argv)[x] == 'i')
	//		set_string_value(argv[x+1], instance);
	//}
}
