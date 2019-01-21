#include <stdlib.h>
#include <unistd.h>
#include "libSNFsync/config.h"
#include "libSNFsync/core.h"

int main (int argc, char **argv){
	char *instance = NULL;
	char tmp;
	int opt;
	printf("Starting!\n");
	printf("Arguments: %i\n", argc);
	 while ((opt = getopt(argc, argv, "i:g")) != -1) {
        switch (opt) {
        case 'i':
            printf("type: %s\n", optarg);
            set_string_value(optarg, &instance);
            break;
        case 'g':
            printf("command: %s\n", optarg);
            generate_config_files(instance);
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
