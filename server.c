#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include "server.h"
#include <semaphore.h>

void client_handler(void *socket_desc)
{


}


int main (int argc, char** argv)
{
	// Variables
	int sock;
	struct sockaddr_in server;
	int mysock;
	// char buff[1024];
	// char input[1024];
	char commandOne[1024];
	// char commandTwo[1024];
	char accountName[1024];
	int rval;
	sharedMemory *accounts; //SHARED MEMORY
	//Account* accounts[20]= malloc(20*sizeof(Account));
	int sizeOfArray = sizeof(sharedMemory);
	printf("Size of accounts array is: %d\n", sizeOfArray);

	// Create shared memory
	int shmid;
	key_t key = NULL;
	// char *shm;

	shmid = shmget(key, sizeOfArray, IPC_CREAT | 0666); // call to create shared memory
	if(shmid < 0)
	{
		perror("shmget");
		exit(1);
	}

	accounts = shmat(shmid, NULL, 0);
	memset((sharedMemory*)accounts, 0, sizeOfArray); //zero out the mem
	shmdt(accounts); //detach shm

	if(accounts == (sharedMemory *) -1)
	{
		perror("shmat");
		exit(1);
	}

	// 1 Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("Failed to create a socket");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(5000);

	// 2 Call bind
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)))
	{
		perror("bind Failed");
		exit(1);
	}

	// 3 Listen
	listen(sock, 100);

	// 4 Accept
	while (true)
	{
		mysock = accept(sock, (struct sockaddr *) 0, 0);
		if(mysock == -1)
		{
			perror("Accept Failed");
		}
		else
		{
			memset(commandOne, 0, sizeof(commandOne)); //set mem

			pid_t childpid;
			childpid = fork();

			if(childpid == 0)
			{
				printf("This is a child process \n");
				accounts = shmat(shmid, NULL, 0);

				memset(commandOne,0,sizeof(commandOne));
				recv(mysock, commandOne, sizeof(commandOne), 0); //get input
				printf("Got the action:%s\n", commandOne);

				if(strcmp(commandOne, "exit") == 0)
				{
					printf("Client has exited\n");
					return 0;
				}

				memset(accountName,0,sizeof(accountName));
				recv(mysock, accountName, sizeof(accountName), 0); //get accountName
				printf("Got the accountName:%s\n", accountName);

				if(strcmp(commandOne, "open") == 0)
				{
					// int exists = 0; //if 1, then you can't make a new account
					int j;
					for(j=0; j<20; j++)
					{
						if(strcmp(accounts->accountsArray[j].name, "accountName") == 0)
						{
							send(mysock,"The account name already exits" , 30, 0 );
							printf("The account name already exits\n");
						}
					}
				}



				shmdt(accounts);

			}
			else
			{
				sleep(1);
				printf("This is a parent process \n");
			}

			printf("Got the message (rval = %d)\n", rval);
			close(mysock);
		}
	}
	return 0;
}