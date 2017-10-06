/* your code goes here. */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <sys/stat.h>
#include <fcntl.h>

char webpage_firstPart[] =
			"HTTP/1.1 200 OK\r\n"
			"Content-type: text/html; charset=UTF -8\r\n\r\n"
			"<!DOCTYPE html>\r\n"
			"<html>\r\n"
				"<body><h1>";

char webpage_secondPart[] =
				"</h1><br>\r\n"
			"</body></html>\r\n";

int main(int argc, char *argv[])
{
 	int sockfd , port;
    	struct sockaddr_in server, client;
	char message[1024];
	char *ipAddr;
	
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
		char *str, *url;
		
				

        	// We first have to accept a TCP connection, connfd is a fresh
        	// handle dedicated to this connection.
        	socklen_t len = (socklen_t) sizeof(client);
        	int connfd = accept(sockfd, (struct sockaddr *) &client, &len);
		ipAddr = inet_ntoa(client.sin_addr);
		
		read(sockfd, buff, 2048);
		
/*	
		ssize_t n = recv(connfd, message, sizeof(message) - 1, 0);
        	if (n == -1) {
            		perror("recv");
            		exit(EXIT_FAILURE);
       		}

        	message[n] = '\0';
  	     	fprintf(stdout, "Received:\n%s\n", message);
	
		char *body;
		body = strstr(message, "\n\r\n\r");			
		
		printf("print out body %s", body);		
*/
		if(connfd == -1)
		{
			perror("Error: connection failed");
		}
		printf("Connected to client...\n");
		/*get info from client */		
	
		fp = fdopen(connfd, "r");		
	
		//Get the request
		
		str = strtok((fgets(buff, sizeof(buff), fp)), " \r\n");
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
		
		fclose(fp);
		/*finished getting info from client*/
		
		/*Function  Logger()*/
		/*Make the Log file*/
		FILE *file;
		time_t timestamp = time(NULL);
    		char responseCode[30];

		file = fopen("Logger.log", "a+");		
		if(file == NULL) {printf("error");}
			
		fprintf(file, "%s"  ,ctime(&timestamp));
		fprintf(file, " : %s", ipAddr);
		fprintf(file, " : %d", port);
		fprintf(file, " : %s" , str);
		fprintf(file, " : %s" , url);

		if(strcmp(str,"GET") == 0 || strcmp(str,"POST")  == 0 || strcmp(str, "HEAD") == 0)
		{
			strcat(responseCode, "200 OK");	
		}
		else
		{
			strcat(responseCode, "400 error");
		}
		fprintf(file, " : %s", responseCode);

		fclose(file);
		
		/*Finished adding to log*/
		
		/*Handle different types of requests*/
		
		//get the body from POST
	
		char buffer[2000];	
	
		strcpy(buffer, webpage_firstPart);		

		if(strcmp(str, "GET") == 0)
		{
			strcat(buffer, "127.0.0.1");
			strcat(buffer, url);
		//	snprintf(buffer, sizeof(buffer),"%d", port);
			strcat(buffer, webpage_secondPart);
			printf("Sending: %s\n", buffer);
		
			send(connfd, buffer, strlen(buffer), 0);
		}
		else if(strcmp(str, "POST") == 0)
		{
			strcat(buffer, "127.0.0.1");
			strcat(buffer, url);
			
//			strcat(buffer, body);

			strcat(buffer, webpage_secondPart);
			send(connfd, buffer, strlen(buffer), 0);
		}
		else if(strcmp(str, "HEAD") == 0)
		{
			send(connfd, buffer, strlen(buffer), 0);
		}
/*
		int fdsend = open(url, O_RDONLY);		
		int nread;	
				
		while((nread = read(fdsend, webpage, sizeof(webpage))) > 0)
    		{
        		write(connfd, webpage, nread);   
    		}

		send(connfd, webpage, strlen(webpage), 0);*/
//		close(fdsend); 
	}
}
