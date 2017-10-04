/* your code goes here. */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
 	int sockfd , port;
    	struct sockaddr_in server, client;
    	char message[512];
	
	if(argc == 1)
	{
		printf("warning disabled!!");
	}        
 
	// Create and bind a TCP socket.
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	port = atoi(argv[1]);	

	if(sockfd < 0)
	{
		perror("error: bind");
		exit(1);
	}

	// Network functions need arguments in network byte order instead of
    	// host byte order. The macros htonl, htons convert the values.

	memset(&server, 0, sizeof(server));
    	server.sin_family = AF_INET;
    	server.sin_addr.s_addr = htonl(INADDR_ANY);
    	server.sin_port = htons(port);

    	if(bind(sockfd, (struct sockaddr *) &server, (socklen_t) sizeof(server)))
	{
		perror("error: bind");
		exit(1);
	}

    	// Before the server can accept messages, it has to listen to the
    	// welcome port. A backlog of one connection is allowed.
    	listen(sockfd, 1);

	for (;;) {
        	// We first have to accept a TCP connection, connfd is a fresh
        	// handle dedicated to this connection.
        	socklen_t len = (socklen_t) sizeof(client);
        	int connfd = accept(sockfd, (struct sockaddr *) &client, &len);

		// Receive from connfd, not sockfd.
        	ssize_t n = recv(connfd, message, sizeof(message) - 1, 0);


		message[n] = '\0';
        	fprintf(stdout, "Received:\n%s\n", message);

		// Convert message to upper case.
        	for (int i = 0; i < n; ++i) message[i] = toupper(message[i]);

		// Send the message back.
        	send(connfd, message, (size_t) n, 0);

		// Close the connection.
        	shutdown(connfd, SHUT_RDWR);
        	close(connfd);
    	}
}
