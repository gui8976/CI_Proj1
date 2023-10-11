
#include "server.h"

#include <stdio.h>

#define DEFAUTADDR "127.0.0.1"
#define BUF_LEN 255

// esta para OBRAS

int main(){

    int sockets, rcv_socket, length;
    struct sockaddr_in local, remote;
    socklen_t addrlen = sizeof(local);
    char buf[BUF_LEN];

    sockets = socket(PF_INET,SOCK_STREAM,0); // cria o pacote


    // incia as informações do pacote do server local
    local.sin_family = AF_INET;
    local.sin_port = htons(MYPORT);
    inet_aton(DEFAUTADDR, &local.sin_addr);

    if(bind(sockets, (struct sockaddr *) &local, addrlen) < 0)
    {
        print("ERROR: Binding failed , ending connection\n");
        return -1;
    }

    if(listen(sockets,3) < 0)
    {
        print("ERROR: couldn't initialize , ending connection\n");
        return -1;
    }
    
    // reads the socket from the client 
    rcv_socket = accept(sockets, (struct sockaddr *) &remote , &addrlen);

    if(rcv_socket <0)
    {
        print("ERROR: couldn't accept the client socket , ending connection\n");
        return -1;
    }

    length = recv(rcv_socket, buf, BUF_LEN, 0);

    if(length <0)
    {
        print("ERROR: couldn't receive the client socket , ending connection\n");
        return -1;
    }
    else print("Data received: %d Bytes\n",length);


    //waits fo the client request for comunication 






}