
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#define MAX_APDU 256
#define MIN_REG_QUANTITY 1
#define MAX_REG_QUANTITY 125

#define Function_WMR 16
#define Function_RMD 3

int Write_multiple_registers(int socket ,u_int16_t Starting_address, u_int16_t quantity, u_int16_t * data);

int Read_holding_registers(int socket ,u_int16_t Starting_address, u_int16_t quantity);


