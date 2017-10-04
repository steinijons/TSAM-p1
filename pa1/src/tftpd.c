#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    	int sockfd, port;
    	struct sockaddr_in server, client;
    	char message[512];
	char* filename;
	char directory[100];
	FILE* file;

    	if(argc < 2) {
        	printf("Please insert a port number!");
        	exit(1);
    	}

    	// Create and bind a UDP socket
    	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	port = atoi(argv[1]);
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;

    	// Network functions need arguments in network byte order instead
    	// of host byte order. The macros htonl, htons convert the
    	// values.
    	server.sin_addr.s_addr = htonl(INADDR_ANY);
    	server.sin_port = htons(port);

    	if(bind(sockfd, (struct sockaddr *) &server, (socklen_t) sizeof(server))) {
       		perror("");
       		exit(1);
    	}

    	for(;;) {
        	// Receive up to one byte less than declared, because it will
        	// be NUL-terminated later.

		socklen_t len = (socklen_t) sizeof(client);
        	ssize_t n = recvfrom(sockfd, message, sizeof(message) - 1,0, (struct sockaddr *) &client, &len);
	
		if(n < 0) {
			printf("ERROR, message from client not recieved");
			exit(1);
		}

		printf("Size of message from client: %zu\n", n);		

		message[n] = '\0';
		
		//RRQ request
		if(message[1] == 1){
			
			//Get the filename requested
			filename = &message[2];

			printf("read request from client: %s\n", filename);

	    		//Get full path of directory for the file requested.
            		strcpy(directory, argv[2]);
	    		strcat(directory, "/");
	    		strcat(directory, filename);
	    		printf("Directory reqeusted: %s\n", directory);

	   		/* char* line = NULL;
	    		size_t len = 0;
            		ssize_t read;	    

			while((read = getline(&line, &len, file)) != -1) {
           	            printf("%s", line);
                        }
                        */
			
			//Open the file requested
	    		file = fopen(directory, "r");
			//check if the file exists
			if(file == NULL) {
				printf("ERROR, file not found!");
				exit(1);
			}

			unsigned short packNr = 1;
			char buffer[512];
			char opcodeFromClient[4];
            		int bytes;
            	
            		for(;;){
				//read from file to buffer
				bytes = fread(buffer + 4, 1, 512, file);		
				if(bytes < 512) {
					printf("End of file, do something here!!");
					break;
				}
				
                		buffer[1] = 3;
                		buffer[2] = packNr >> 8;
                		buffer[3] = packNr;
			
		                fprintf(stdout, "Sending package of size: %d\n", bytes);
        		        fprintf(stdout, "Sending package Nr: %d\n", packNr);
			
				//send buffer to client and wait for ACK response
				//in example1 we get everything but the last package. We always get stuck on package nr 7 when trying example2, 
				//don't klnow why. ran out of time 
				do {
					sendto(sockfd, buffer, bytes + 4, 0, (struct sockaddr *) &client, len);
                        	
					recvfrom(sockfd, opcodeFromClient, sizeof(message) - 1,0, (struct sockaddr *) &client, &len);
				
					if(opcodeFromClient[1] == 4) {
						packNr++;
						break;
					}
				}
				while(opcodeFromClient[1] == 4);		
         	       }	
		}
		if(message[1] == 2) {
			printf("ERROR, WRQ not allowed!");
			exit(1);
		}
		if(message[1] == 3) {
			printf("ERROR, DATA not allowed!");
			exit(1);
		}
		if(message[1] == 5) {
			printf("blabla");
			exit(1);
		}
    	}
}

