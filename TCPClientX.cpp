#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 12000

int main()
{
	//initializing of standard variables
	int clientSocket;
	char str[1024] = "Client X: Alice", currentTime2[84] = "", buffer[1024] = "X: Alice";

	//initializing of nonstandard variables
	struct sockaddr_in serverAddress;
	
	//creating the socket
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0) //if this is true there has been a connection error
	{
		printf("A connection error has occurred.\n");
		exit(1);
	}
	
	//initializing information, assigning IP and PORT
	memset(&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//connecting the client to the server
	if(connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) //if this is true there has been a connection error
	{
		printf("A connection error has occurred.\n");
		exit(1);
	}
	
	//if the message (stored in buffer[]) with the client's name was succesfully sent to the server, print out (on the client's terminal) that the message was sent along with the message sent (stored in str[])
	while(1)
	{
		if(send(clientSocket, buffer, strlen(buffer), 0))
		{
			printf("The message has been sent. The message is: %s.\n", str);
		}
		
		//priting out the message recieved from the server
		if(recv(clientSocket, currentTime2, 84, 0))
		{
			printf("The time is %s\n", currentTime2);
			return 0;
		}
		if(strcmp(buffer, ":exit") == 0)
		{
			close(clientSocket);
			printf("Client has been disconnected from the server.\n");
			exit(1);
		}
		if(recv(clientSocket, buffer, 1024, 0) < 0)
		{
			
		}
		else
		{
		
		}
	}
	return 0;
}
	

