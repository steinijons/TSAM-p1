#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    //10519
    printf("argc: %d", argc);
     
    int sockfd;
    int port = atoi(argv[1]);
    struct sockaddr_in server, client;
    char message[512];

    // Create and bind a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server ,0, sizeof(server));
    server.sin_family = AF_INET;
    // Network functions need arguments in network byte order instead
    // of host byte order. The macros htonl, htons convert the
    // values.

    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    bind(sockfd, (struct sockaddr *) &server, (socklen_t) sizeof(server));

    for (;;) {
	// Receive up to one byte less than declared, because it will
        // be NUL-terminated later.
        socklen_t len = (socklen_t) sizeof(client);
        ssize_t n = recvfrom(sockfd, message, sizeof(message) - 1, 0, (struct sockaddr *) &client, &len);
        if (n >= 0) {
            message[n] = '\0';
            fprintf(stdout, "Received:\n%s\n", message);
            fflush(stdout);

            sendto(sockfd, message, (size_t) n, 0,
                   (struct sockaddr *) &client, len);
        } else {
            // Error or timeout. Check errno == EAGAIN or
            // errno == EWOULDBLOCK to check whether a timeout happe	    }
    }

}
