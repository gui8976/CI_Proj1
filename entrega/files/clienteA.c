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

#define SERVER_ADDR "127.0.0.1"
#define REMOTE_ADDR "10.227.113.1"
#define PORT 502

int main()
{
    // Aliena A)

    // writes in 121 the value 0x41
    uint16_t Starting_address_write = 120; // -1 cause physical adress starts at 1
    uint16_t Number_of_address_to_write = 1;
    uint16_t data1[] = {0x41}; // data to write
    int result1 = 0;
    char ip[] = SERVER_ADDR;
    int socket = connect_to_modbus_tcp(PORT, ip);

    // writes the data to the register pretended
    result1 = write_multiple_registers(socket, Starting_address_write, Number_of_address_to_write, data1);
    if(result1 != 0){
        printf("RESULT : %d\n",result1);
        return result1;
    }

    // Aliena B)

    uint16_t Starting_address_read_1 = 121;
    uint16_t Number_of_address_to_read_1 = 4;
    uint16_t data_rcv[Number_of_address_to_read_1];
    int result2 = 0;

    // reads the data from the registers pretended

    result2 = read_holding_registers(socket, Starting_address_read_1, Number_of_address_to_read_1, data_rcv);
    if(result2 != 0){
        printf("RESULT : %d\n",result2);
        return result2;
    }

    // Aliena C)
    uint16_t Starting_address_read_2 = 125;
    uint16_t Number_of_address_to_read_2 = 1;
    uint16_t B = 0;
    uint16_t C = 0;

    // reads the data from the register pretended

    result2 = read_holding_registers(socket, Starting_address_read_2, Number_of_address_to_read_2, &B);   
    if(result2 != 0){
        printf("RESULT : %d\n",result2);
        return result2;
    }

    if (B == 0)
    {
        C = 9999;
    }

    else
    {
        C = data_rcv[0] + data_rcv[3];
    }

    // Aliena D)
    // writes the C value in the register pretended
    uint16_t Starting_address_write_1 = 126; // -1 cause physical adress starts at 1
    uint16_t Number_of_address_to_write_1 = 1;
    int result3 = 0;
    uint16_t data2[] = {C}; // data to write

    result3 = write_multiple_registers(socket, Starting_address_write_1, Number_of_address_to_write_1, data2);
    if(result3 != 0){
        printf("RESULT : %d\n",result3);
        return result3;
    }
    disconnect_from_modbus_tcp(socket);

    // Aliena E)
    // connects to the  remote host
    char remote[] = REMOTE_ADDR;
    int socket1 = connect_to_modbus_tcp(PORT, remote);
    int result4 = 0;

    uint16_t Starting_address_write_2 = 127; // -1 cause physical adress starts at 1
    uint16_t Number_of_address_to_write_2 = 1;
    // Writes the data to the register pretended


   result4 = write_multiple_registers(socket, Starting_address_write_2, Number_of_address_to_write_2, data2); 
    if(result4 != 0){
        printf("RESULT : %d\n",result4);
        return result4;
    }
    // disconnects from the remote host
    disconnect_from_modbus_tcp(socket1);
    return 0;  
}