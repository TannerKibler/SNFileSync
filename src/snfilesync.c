#include <stdlib.h>
#include <unistd.h>
#include "libSNFsync/config.h"
#include "libSNFsync/core.h"

int main (int argc, char **argv){
	char *instance = NULL, *pass = NULL;
	char username[40];
	int opt;
	printf("Starting!\n");
	printf("Arguments: %i\n", argc);
	 while ((opt = getopt(argc, argv, "i:g:r")) != -1) {
        switch (opt) {
        case 'i':
            printf("type: %s\n", optarg);
            set_string_value(optarg, &instance);
            break;
        case 'g':
			printf("Username: ");
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
            break;
		case 'r':
			printf("\nForcing check for files\n---------------------\n");
			//run_file_checker_for_instance(instance);
			break;
    }
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
