
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MIN_REG_ADDRESS 0
#define MAX_REG_ADDRESS 0xFFFF
#define MIN_REG_QUANTITY 1
#define MAX_REG_TO_READ_QUANTITY 0x007D
#define MAX_REG_TO_WRITE_QUANTITY 0x007B

#define FUNCTION_WMR 16
#define FUNCTION_RMD 3

int write_multiple_registers(int socketfd ,u_int16_t Starting_address, u_int16_t quantity, u_int16_t * data);

int read_holding_registers(int socketfd ,u_int16_t Starting_address, u_int16_t quantity, u_int16_t * data_read);


