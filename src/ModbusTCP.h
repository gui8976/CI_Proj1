
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#define MAX_HEADER_SIZE 7

// necessita de um IP e de um socket
int connect_to_ModbusTCP(int port,char* ip_port);

// >0 good    <bad
int disconnect_from_ModbusTCP(int sockets);

int Send_Modbus_data(int sockets, uint8_t* response, int size_response);

int Receive_Modbus_request(int sockets, uint8_t* request, int size_request);

int send_Modbus(int sockets, int size_request , uint8_t* request);

int receive_Modbus(int sockets, uint8_t* request);