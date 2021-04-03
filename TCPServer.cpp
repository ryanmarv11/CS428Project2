#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>

#define PORT 4444

int main()
{
	//initializing of standard variables
	int numMessagesReceived = 0, opt = 1, numMaxClients = 30;
	int clientSocketArray[30];
	int firstMilleseconds, firstFd, sockfd, max_sd, newSocket, activity, valread, sd, addrlen;
	char backupBuffer[1024], buffer[1024], currentTime2[84], str[100] = "X: Alice recieved before Y: Bob";
	
	//initializing of non-standard variables
	struct timeval first;
	struct sockaddr_in serverAddress;
	socklen_t addr_size;
	pid_t childpid;
	fd_set readfds;
	

	//initializing all clientSocketArray indices to 0, meaning they're unchecked
	for(int i = 0; i < numMaxClients; i++)
	{
		clientSocketArray[i] = 0;
	}
	
	//creation of socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) //if this is true, socket creation has failed
	{
		printf("Socket Creation Failed.\n");
		exit(1);
	}

	//initializing server info
	//first up is assigning the IP and PORT
	memset(&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//now to bind the newly created socket
	if(bind(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) //if this is true, the binding has failed
	{
		printf("Binding has failed.\n");
	}
	int m = strlen(str);
	if(listen(sockfd, 10) < 0)
	{
		printf("It's time to listen up.\n");
		return 1;
	}
	addrlen = sizeof(serverAddress);
	while(1)
	{
		//clearing the socket set
		FD_ZERO(&readfds);

		//adding socket (sockfd) to the set
		max_sd = sockfd;

		//adding child sockets to the set
		for(int i = 0; i < numMaxClients; i++)
		{
			//socket descriptor
			sd = clientSocketArray[i];
			if(sd > 0) //if this is true, the socket descriptor is valid, so add it do the list
			{
				FD_SET(sd, &readfds);
			}
			
			if(sd > max_sd)
			{
				max_sd = sd;
			}
		}
		
		//waiting for an activity on one of the sockets, timeout is NULL so waiting time is indefinite
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if((activity < 0) && (errno != EINTR)) //if both of these are true, there has been a select error
		{
			printf("Select error");
		}
		if(FD_ISSET(sockfd, &readfds))
		{
			if((newSocket = accept(sockfd, (struct sockaddr *) &serverAddress, (socklen_t*) &addrlen)) < 0)
			{
				exit(EXIT_FAILURE);
			}
			
			//the connection has been accepted, informing the user of the socket fd, the IP address, and the PORT
		printf("Connection has been succesfully accepted. The socket fd is :%d, the IP address is: %s, the PORT is: %d.\n", newSocket, inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
		
		//adding new socket to the current array of sockets
		for(int i = 0; i < numMaxClients; i++)
		{
			if(clientSocketArray[i] == 0) //if this is true, position is empty
			{
				clientSocketArray[i] = newSocket;
				//get the time of the message recieved from the client
				struct timeval now;
				gettimeofday(&now, NULL);
				//get the time of day specifically in milliseconds
				int milli2 = now.tv_usec / 1000;
				//newerBuffer[] allows for easy to read formatting
				char newerBuffer[80];
				strftime(newerBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));
				//currentTime[] stores the time
				char currentTime2[84] = "";
				//receiving data from client, buffering from both clients x and y
				recv(newSocket, buffer, 1024, 0);
				sprintf(currentTime2, "%s:%03d", newerBuffer, milli2);
				printf("Client%s\n, ", buffer);
				
				if(numMessagesReceived > 0)
				{
					char blank[100];
					int comparisonValue = timercmp(&now, &first, >);
					if(comparisonValue > 0)
					{
						sprintf(blank, "%s recieved before %s", backupBuffer, buffer);
					}
					else
					{
						sprintf(blank, "%s recieved before %s", buffer, backupBuffer);
					}
					strncpy(str, blank, 100);
					if(send(newSocket, str, strlen(str), 0) != strlen(str))
					{
						printf("It's time to send.\n");
					}

					//String sent to Client X and Client y as to which was recieved first
					send(firstFd, str, strlen(str), 0);
					//Printed ACK for server after the sring was send by the sever and received form the client
					printf("Sent acknowledgment to both X and Y\n");
					return 0;
				}
				else
				{
					strncpy(backupBuffer, buffer, 1024);
					numMessagesReceived++;
					first = now;
					firstMilleseconds = milli2;
					firstFd = newSocket;
				}
			break;
			}
			}
			}

			//If we haven't "breaked" yet, it's an IO operation on another socket
			for(int i = 0; i < numMaxClients; i++)
			{
				sd = clientSocketArray[i];
				if(FD_ISSET(sd, &readfds))  //if this is true it was for closing
				{
					//reading the incoming message
					if((valread = read(sd, buffer, 1024)) == 0)
					{
						getpeername(sd, (struct sockaddr*)&serverAddress, (socklen_t *)&addrlen);
						printf("Host has been disconnected. The IP address is :%s, the PORT is:%d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
					//closing the socket and marking it as a 0 in the list
					close(sd);
					clientSocketArray[i] = 0;
				}
				else
				{
					//setting the string terminating NULL byte at the end of the data read
					buffer[valread] = '\0';
					send(sd, buffer, strlen(buffer), 0);
				}
			}
		}
	}
	return 0;
}





















		











	
