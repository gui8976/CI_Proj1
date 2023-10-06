#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "modbusAPP.h"
#include "modbusTCP.h"



// write a function that sets up the Modbus packet
// returns 0 if it was successful
// writes multiples registers in a TCP socket
int Write_multiple_registers(int sockets, u_int16_t Starting_address, u_int16_t quantity, u_int16_t *data)
{

    // APDU length = 6(incial data protocol) + 2 * quantity
    int APDU_length = 6 + 2 * quantity;
    int send = 0;
    int received = 0;


    uint8_t request[APDU_length];

    if (quantity < MIN_REG_QUANTITY || quantity > MAX_REG_QUANTITY)
    {
        printf("ERROR: invalid quantity of registers\n");
        return -1;
    }

    // build the header
    request[0] = (uint8_t)Function_WMR;              // function code
    request[1] = (uint8_t)(Starting_address >> 8);   // high byte of starting address
    request[2] = (uint8_t)(Starting_address & 0xFF); // low byte starting address
    request[3] = (uint8_t)(quantity >> 8);           // high byte of quantity of registers
    request[4] = (uint8_t)(quantity & 0xFF);         // low byte of quantity of registers
    request[5] = (uint8_t)(quantity * 2);            // number of bytes in value field

    // build the data field
    for (int i = 0; i < quantity; i++)
    {
        request[6 + i * 2] = (uint8_t)(data[i] >> 8);     // high byte of register value
        request[7 + i * 2] = (uint8_t)(data[i] & 0x00FF); // low byte of register value
    }


    send = send_Modbus(sockets, APDU_length, request);
    
//    send = send_Modbus_request(ServerAddrs, port, request, APDU_length, request);
    if(APDU_length != send)
    {
        printf("ERROR: couldn't send the request to write multiple registers\n");
        return -1;
    }

    // receives the response
    received = receive_Modbus(sockets, request);
    if(received < 0)
    {
        printf("ERROR: couldn't receive the response\n");
        return -1;
    }

    // checks if the response is valid
    if(request[0] & 0x80)
    {
        printf("ERROR: invalid response\n");
        return -1;
    }
    return received;
}
// reads the holding registers
// returns 0 if it was successful
// reads multiples registers in a TCP socket

int Read_holding_registers(int sockets ,u_int16_t Starting_address, u_int16_t quantity)
{
    // APDU length = 5 -> 1 byte for function code + 2 bytes for starting address + 2 bytes for quantity of registers
    int APDU_length = 5;
    int send = 0;
    int received = 0;

    uint8_t request[APDU_length];

    if (quantity < MIN_REG_QUANTITY || quantity > MAX_REG_QUANTITY)
    {
        printf("ERROR: invalid quantity of registers\n");
        return -1;
    }

    //build the header
    request[0] = (uint8_t)Function_RMD;              // function code
    request[1] = (uint8_t)(Starting_address >> 8);   // high byte of starting address
    request[2] = (uint8_t)(Starting_address & 0xFF); // low byte starting address
    request[3] = (uint8_t)(quantity >> 8);           // high byte of quantity of registers
    request[4] = (uint8_t)(quantity & 0xFF);         // low byte of quantity of registers

    send = send_Modbus(sockets, APDU_length, request);

    // checks if there was no data lost in the sending
    if(APDU_length != send)
    {
        printf("ERROR: couldn't send the request to write multiple registers\n");
        return -1;
    }

    // receives the response
    received = receive_Modbus(sockets,request);
    // checks if there was no data lost in the receiving
    if(received < 0)
    {
        printf("ERROR: couldn't receive the response\n");
        return -1;
    }

    // checks if the response is valid
    if(request[0] & 0x80)
    {
        printf("ERROR: invalid response\n");
        return -1;
    }

    return received;

}


