#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <pthread.h>

#define reset   "\x1b[0m"
#define red     "\x1b[31m"
#define green   "\x1b[32m"
#define yellow  "\x1b[33m"

int client(char** argv)
{
	// Variables
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	// char buff[1024];

	// Create connection
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror(red "Socket failed: " reset);
		exit(1);
	}

	server.sin_family = AF_INET;

	hp = gethostbyname(argv[1]);
	if(hp == 0)
	{
		perror(red "gethostbyname failed: " reset);
		close(sock);
		exit(1);
	}

	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(5000);

	if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		perror(red "Connect failed: " reset);
		close(sock);
		exit(1);
	}

	// Give user options and send them to server
	char input[109];
	char account[100];

	printf(green "Connected to the server. Please enter one of the following:\n" reset);
	printf("> open 'accountname'\n> start 'accountname'\n> exit\n");
	scanf("%s",input);

	while(strcmp(input, "open") != 0 && strcmp(input, "start") != 0 && strcmp(input, "exit") != 0)
	{
		printf(red "ERROR: Invalid input, please enter one of the following:\n");
		printf("1.open 'accountname'\n2.start 'accountname'\n3.exit\n");
		scanf("%s",input);
	}

	if(send(sock, input, sizeof(input), 0) < 0) //sending data to sock
	{
		perror(red "Send failed: " reset);
		close(sock);
		exit(1);
	}

	// printf("Sent %s\n", input);

	//take account name for open/create commands
	if(strcmp(input, "exit") != 0)
	{
		scanf("%s",account);

		if(send(sock, account, sizeof(account), 0) < 0) //sending data to sock
		{
			perror(red "Send failed: ");
			close(sock);
			exit(1);
		}

		printf(green "Sending: '");
		printf(yellow "%s", input);
		printf("%s" green "'\n" reset, account);

		strcat(input, " ");
		strcat(input, account);
		strcat(input, "\0");
	}
	else
	{
		printf("You have exited.\n");

		char rcv[1024];
		memset(rcv,0,sizeof(rcv));
		recv(sock, (void*)recv, sizeof(rcv), 0); //get input
		printf("Got the message:%s\n", rcv);

		return 0;
	}

	printf("%s\n",input);
	close(sock);

	return 0;
}


void *listenFunc()
{
	while (1)
	{
		printf(green "\nI am listening for input\n" reset);
		sleep(15);
	}
}


int main(int argc, char** argv)
{
	// Error for incorrect input
	if (argc != 2)
	{
		printf(red "You're a dumbass\n" reset);
		exit(1);
	}

	pthread_t comm_listener;
	if (pthread_create(&comm_listener, NULL, listenFunc, NULL))
	{
		perror(red "ERROR creating thread: ");
		exit(0);
	}

	client(argv);
}