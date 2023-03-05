#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

struct message {
    unsigned int name_size;
    unsigned int file_size;
    unsigned char data[0];
}__attribute__((packed));

int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in addr;
    struct message *message;
    size_t message_len;
    
    if ( argc != 4 ) {
	printf("Usage: %s <ip> <file_name> <message>\n",argv[0]);
	return 0;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if( sd < 0 ) {
	perror("Error calling socket");
	return __LINE__;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    if ( (-1) == connect(sd,(struct sockaddr *)&addr, sizeof(addr))) {
	perror("connect");
	close(sd);
	return __LINE__;
    }
    
    message_len = sizeof(struct message) + strlen(argv[2]) + strlen(argv[3]);
    message = (struct message*) malloc(message_len);
    if ( message == NULL ) {
	perror("malloc");
	close(sd);
	return __LINE__;
    }
    message->name_size = strlen(argv[2]);
    message->file_size = strlen(argv[3]);

    memcpy(message->data, argv[2], strlen(argv[2]));
    memcpy(message->data + strlen(argv[2]), argv[3], strlen(argv[3]));
    if ( write(sd, message, message_len) != message_len ) fprintf(stderr,"Send problem\n");
    close(sd);
    free(message);
    return 0;
}
