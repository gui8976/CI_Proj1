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
#define REMOTEADDR " 10.227.113.1"
#define PORT 502

int main()
{
    // Aliena A)

    uint16_t Starting_address_write = 122 - 1; // -1 cause physical adress starts at 1
    uint16_t Number_of_address_to_write = 1;
    uint16_t data1[] = {0x41}; // data to write
    int result1 = 0;
    int socket = connect_to_modbus_tcp(PORT, SERVERADDR);

    // writes the data to the register pretended
    if (result1 = write_multiple_registers(socket, Starting_address_write, Number_of_address_to_write, data1) < 0)
    {
        printf("ERROR: couldn't write the registers\n");
        return -1;
    }

    else
        return result1;

    // Aliena B)

    uint16_t Starting_address_read = 123-1;
    uint16_t Number_of_address_to_read = 4;
    uint16_t data_rcv[Number_of_address_to_read];
    int result2 = 0;

    // reads the data from the registers pretended
    if (result2 = read_holding_registers(socket, Starting_address_read, Number_of_address_to_read, data_rcv) < 0)
    {
        printf("ERROR: couldn't read the registers\n");
        return -1;
    }

    else
        return result2;

    // Aliena C)
    uint16_t Starting_address_read = 127-1;
    uint16_t Number_of_address_to_read = 1;
    uint16_t B = 0;
    uint16_t C = 0;

    //reads the data from the register pretended
    if (result2 = read_holding_registers(socket, Starting_address_read, Number_of_address_to_read, &B) < 0)
    {
        printf("ERROR: couldn't read the registers\n");
        return -1;
    }

    else
        return result2;

    if(B == 0)
    {
        C = 9999;
    }

    else
    {
        C = data_rcv[0] + data_rcv[3];
    }

    // Aliena D)
    // writes the C value in the register pretended
    uint16_t Starting_address_write = 128 - 1; // -1 cause physical adress starts at 1
    uint16_t Number_of_address_to_write = 1;
    int result3 = 0;
    uint16_t data2[] = {C}; // data to write

    if (result3 = write_multiple_registers(socket, Starting_address_write, Number_of_address_to_write, data2) < 0)
    {
        printf("ERROR: couldn't write the registers\n");
        return -1;
    }

    else
        return result3;
    
    disconnect_from_modbus_tcp(socket);

    // Aliena E)
    // connects to the  remote host
    int socket = connect_to_modbus_tcp(PORT, REMOTEADDR);
    int result = 4;

    uint16_t Starting_address_write = 129 - 1; // -1 cause physical adress starts at 1
    uint16_t Number_of_address_to_write = 1;
    // Writes the data to the register pretended
    if (result1 = write_multiple_registers(socket, Starting_address_write, Number_of_address_to_write, data2) < 0)
    {
        printf("ERROR: couldn't write the registers\n");
        return -1;
    }

    else
        return result1;

    // disconnects from the remote host
    disconnect_from_modbus_tcp(socket);
}