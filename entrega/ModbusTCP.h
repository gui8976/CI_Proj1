
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#define MAX_MBAP_HEADER_SIZE 7
#define UNIT_ID 0x01;

// necessita de um IP e de um socket
int connect_to_modbus_tcp(int port,char* ip_port);

// >0 good    <bad
int disconnect_from_modbus_tcp(int socketfd);

int send_modbus_data(int socketfd, uint8_t* response, int size_response);

int receive_modbus_data(int socketfd, uint8_t* request, int size_request);

int send_modbus(int socketfd, int size_request , uint8_t* request);

int receive_modbus(int socketfd, uint8_t* request);