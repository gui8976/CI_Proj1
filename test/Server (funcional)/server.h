#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MYPORT 2222


struct sockaddr_in
{
    sa_family_t sin_family; // address family: AF_INET
    u_int16_t sin_port;     // port in network byteorder
    struct in_addr sin_addr; // internet address
};

struct in_addr
{
    u_int32_t s_addr; /* 32 bit address in network byte order */
};


//creates a simple socket
int socket(int domain, int type, int protocol);

// attaches an adress to a socket
int bind(int sockfd, struct sockaddr *addr, socklen_t addrlen);

//waits for the client so send a message 
int listen(int socket, int backlog);

//checks if the server can connect and receive the socket 
int accept(int socket, struct sockaddr *addr, socklen_t *addrlen );

//int send(int socket, const void *buf, size_t len, int flags );

//int recv(int socket, const void *buf, size_t len, int flags);

// converte dados para o respetivo formato

/*
unsigned long int htonl(unsigned long int hostlong);

unsigned short int htons ( unsigned short int hostshort );

unsigned long int ntohl ( unsigned long int netlong );

unsigned short int ntohs ( unsigned short int netshort );

long inet_aton( char *, struct in_addr * );

char *inet_ntoa( struct in addr );

*/

