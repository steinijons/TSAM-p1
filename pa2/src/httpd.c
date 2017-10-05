/* your code goes here. */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/stat.h>
#include <fcntl.h>

char webpage[] =
"HTTP/1.1 200 OK\r\n"
"Content-type: text/html; charset=UTF -8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html>\r\n"
"<body><h1>An IP address should be here, plus the port number</h1><br>\r\n"
"</body></html>\r\n";

int main(int argc, char *argv[])
{
 	int sockfd , port;
    	struct sockaddr_in server, client;
    	char message[512];

	
	
	for(int i = 0; i < argc; i++)
	{
		printf("%s",argv[i]);
	}	


	if(argc == 1)
	{
		printf("warning disabled!!");
	}        
 
	// Create and bind a TCP socket.
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	port = atoi(argv[1]);	

	if(sockfd < 0)
	{
		perror("error: can't open socket");
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
		perror("error: can't bind socket");
		exit(1);
	}

    	// Before the server can accept messages, it has to listen to the
    	// welcome port. A backlog of one connection is allowed.
    	listen(sockfd, 1);

	for (;;) {
		FILE *fp;
		char buff[2048];
		char buff2[200];
		char *str, *url;
		
        	// We first have to accept a TCP connection, connfd is a fresh
        	// handle dedicated to this connection.
        	socklen_t len = (socklen_t) sizeof(client);
        	int connfd = accept(sockfd, (struct sockaddr *) &client, &len);
		
		if(connfd == -1)
		{
			perror("Error: connection failed");
		}
			
		fp = fdopen(connfd, "r");
		//read the first line and check what request we are suppose to handle
		
		str = fgets(buff, sizeof(buff), fp);		
		fprintf(stderr, "Request line from client: %s\n", str);
		//Get the request
		str = strtok(str, " \r\n");
		fprintf(stderr, "Request: %s\n", str);
		//Get the url from client
		url = strtok(NULL, " \r\n");
		if(url == NULL)
		{
			fprintf(stderr, "No URL\n");
		}		

		if(url[0]  == '/')
		{
			url = &url[1];
		}	
		fprintf(stderr, "url: %s\n", url);
		
		int fdsend = open(url, O_RDONLY);		
		int nread;
/*
		sprintf(buff2, "%s"
				"Content-type: %s\r\n\r\n", "HTTP/1.1 200 OK\r\n",

				"<html>"
				"<header><title>This is title</title></header>"
				"<body>"
				"Hello World"
				"</body>"
				"</html>");
		write(connfd, buff2, strlen(buff2));	
	*/	
				
		while ( (nread = read(fdsend, webpage, sizeof(webpage))) > 0)
    		{
        		write(connfd, webpage, nread);   
    		}

		send(connfd, webpage, strlen(webpage), 0);
		close(fdsend); 
	}
}
