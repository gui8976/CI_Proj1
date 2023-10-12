#include <stdint.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ModbusAPP.h"
#include "ModbusTCP.h"

#define SERVERADDR "127.0.0.1"
#define PORT 502

int main(void)
{
    uint16_t Starting_address_read = 0;
    uint16_t Number_of_address_to_read = 10;
    uint16_t Starting_address_write = 0;
    uint16_t Number_of_address_to_write = 9;


    uint16_t data_rcv[Number_of_address_to_read];
    int check_error;
    uint16_t data1[] = {0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49};

    int sockets = connect_to_modbus_tcp(PORT, SERVER_ADDR);

    if ((check_error = read_holding_registers(sockets, Starting_address_read, Number_of_address_to_read ,data_rcv)) < 0)
    {
        printf("ERROR: couldn't read the registers\n");
        return -1;
    }
    printf("\n");
    printf("Reading registers\n");

    //writes the registers
    if ((check_error = write_multiple_registers(sockets, Starting_address_write, Number_of_address_to_write, data1)) < 0)
    {
        printf("ERROR: couldn't write the registers\n");
        return -1;
    }
    printf("Success in the connection\n");

    disconnect_from_modbus_tcp(sockets);

    printf("Disconnecting...\n");
    return 0;
}