
#include "server.h"

#include <stdio.h>

#define DEFAUTADDR "127.0.0.1"
#define INPUTDATA "Ola estou aqui"



// esta para OBRAS

int main(){

    int sockets, length;
    struct sockaddr_in server;
    socklen_t addrlen = sizeof(server);

    sockets = socket(PF_INET,SOCK_STREAM,0); // cria o pacote


    // incia as informações do pacote do server local
    server.sin_family = AF_INET;
    server.sin_port = htons(MYPORT);
    inet_aton(DEFAUTADDR, &server.sin_addr);

    if(connect(sockets, (struct sockaddr *) &server, addrlen)<0)
    {
        print("ERROR: couldn't connect to the client server , ending connection\n");  
        return -1; 
    }
    else print("Connected");

    // INPUT DATA

    length = send(sockets, INPUTDATA, strlen(INPUTDATA)+1, 0);
    if(length < 0)
    {
        print("ERROR: couldn't send data, ending connection\n");          
    } else print("Sent %d Bytes of Data",length);

}