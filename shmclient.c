#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHSIZE 100

int main (int argc, char *argv[])
{
	int shmid;
	key_t key;
	char *shm;
	char *s;

	key = 9876;

	shmid = shmget(key, SHSIZE, 0666);
	if(shmid < 0)
	{
		perror("ERROR with shget: ");
		exit(1);
	}

	shm = shmat(shmid, NULL, 0);

	if(shm == (char *) -1)
	{
		perror("ERROR with shmat: ");
		exit(1);
	}

	for(s = shm; *s != 0; s++)
		printf("%c",*s);

	printf("\n");
	// shmdt(accounts);
	*shm = '*';

	return 0;
}