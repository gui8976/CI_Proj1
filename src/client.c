
// simple program to check if the MODBUSAPP works
// this program is not used in the final project

#include <stdio.h>
#include "ModbusAPP.h"
#include "ModbusTCP.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

// includes do lucas
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVERADDR "127.0.0.1"
#define PORT 502
#define STARTING_R 0
#define NUMBER_OF_R 2

int main(void)
{
    /*
        int connected = connect_to_ModbusTCP(PORT, SERVERADDR);
        if (connected < 0)
        {
            printf("ERROR: couldn't connect to the server, ending connection\n");
            return -1;
        }
        printf("Success in the connection\n");

        disconnect_from_ModbusTCP(connected );
        printf("Disconnecting...\n");

        return 0;
    */

    uint16_t data[NUMBER_OF_R];
    int check_error;
    u_int16_t sum = 3;

    int sockets = connect_to_ModbusTCP(PORT, SERVERADDR);

    if ((check_error = Read_holding_registers(sockets, STARTING_R, NUMBER_OF_R)) < 0)
    {
        printf("ERROR: couldn't read the registers\n");
        return -1;
    }
    printf("\n");
    printf("Reading registers\n");

    if ((check_error = Write_multiple_registers(sockets, STARTING_R, NUMBER_OF_R, &sum)) < 0)
    {
        printf("ERROR: couldn't write the registers\n");
        return -1;
    }
    printf("Success in the connection\n");
    disconnect_from_ModbusTCP(sockets);
    printf("Disconnecting...\n");
    return 0;
}