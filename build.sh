#! /bin/sh

rm /home/arch/code/c/SNFileSync/snfilesync

printf "**********************************************************************************************************************\n\n"

gcc -Wall -c -g /home/arch/code/c/SNFileSync/src/snfilesync.c -o /home/arch/code/c/SNFileSync/bin/snfilesync.o
gcc -Wall -c -g /home/arch/code/c/SNFileSync/src/libSNFsync/core.c -o /home/arch/code/c/SNFileSync/bin/static/core.o
gcc -Wall -c -g -lcurl /home/arch/code/c/SNFileSync/src/libSNFsync/sn_instance.c -o /home/arch/code/c/SNFileSync/bin/static/sn_instance.o
gcc -Wall -c -g /home/arch/code/c/SNFileSync/src/libSNFsync/file.c -o /home/arch/code/c/SNFileSync/bin/static/file.o
gcc -Wall -c -g /home/arch/code/c/SNFileSync/src/libSNFsync/config.c -o /home/arch/code/c/SNFileSync/bin/static/config.o
gcc -Wall -c -g /home/arch/code/c/SNFileSync/src/libSNFsync/json.c -o /home/arch/code/c/SNFileSync/bin/static/json.o
ar rcs /home/arch/code/c/SNFileSync/bin/static/libSNFsync.a /home/arch/code/c/SNFileSync/bin/static/core.o /home/arch/code/c/SNFileSync/bin/static/file.o /home/arch/code/c/SNFileSync/bin/static/sn_instance.o /home/arch/code/c/SNFileSync/bin/static/config.o /home/arch/code/c/SNFileSync/bin/static/json.o
 
gcc -Wall -g -lcurl /home/arch/code/c/SNFileSync/bin/snfilesync.o -L/home/arch/code/c/SNFileSync/bin/static -lSNFsync -o /home/arch/code/c/SNFileSync/snfilesync

printf "\n\n**********************************************************************************************************************\n\n"

printf "**********************************************************************************************************************\n\n"

/home/arch/code/c/SNFileSync/snfilesync -i dev69432 -g

printf "\n\n**********************************************************************************************************************\n\n"
