#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "ModbusAP.h"
#include "ModbusTCP.h"

// #define DEBUG

#ifdef DEBUG

#define PRINT(...) printf(__VA_ARGS__)

#else

#define PRINT(...)

#endif

int connect_to_server(int port, char *ip_port)
{
    return connect_to_modbus_tcp(port, ip_port);
}

void disconnect_from_server(int socketfd)
{
    disconnect_from_modbus_tcp(socketfd);
}

// write a function that sets up the Modbus packet
// returns 0 if it was successful
// writes multiples registers in a TCP socket
int write_multiple_registers(int socketfd, u_int16_t starting_address, u_int16_t quantity, u_int16_t *data)
{

    if (socketfd < 0)
    {
        PRINT("ERROR: invalid socket\n");
        return -1;
    }
    if (starting_address < MIN_REG_ADDRESS || starting_address > MAX_REG_ADDRESS)
    {
        PRINT("ERROR: invalid starting address\n");
        return -1;
    }
    if (starting_address + quantity > MAX_REG_ADDRESS)
    {
        PRINT("ERROR: invalid starting address\n");
        return -1;
    }
    if (quantity < MIN_REG_QUANTITY || quantity > MAX_REG_TO_WRITE_QUANTITY)
    {
        PRINT("ERROR: invalid quantity of registers\n");
        return -1;
    }
    if (data == NULL)
    {
        PRINT("ERROR: invalid data array\n");
        return -1;
    }

    // APDU length = 6(incial data protocol) + 2 * quantity
    int apdu_length = 6 + 2 * quantity;
    int send = 0;
    int received = 0;

    uint8_t request[apdu_length];

    // build the header
    request[0] = (uint8_t)FUNCTION_WMR;              // function code
    request[1] = (uint8_t)(starting_address >> 8);   // high byte of starting address
    request[2] = (uint8_t)(starting_address & 0xFF); // low byte starting address
    request[3] = (uint8_t)(quantity >> 8);           // high byte of quantity of registers
    request[4] = (uint8_t)(quantity & 0xFF);         // low byte of quantity of registers
    request[5] = (uint8_t)(quantity * 2);            // number of bytes in value field

    // build the data field
    for (int i = 0; i < quantity; i++)
    {
        request[6 + i * 2] = (uint8_t)(data[i] >> 8);     // high byte of register value
        request[7 + i * 2] = (uint8_t)(data[i] & 0x00FF); // low byte of register value
    }

    send = send_modbus(socketfd, apdu_length, request);

    //    send = send_Modbus_request(ServerAddrs, port, request, APDU_length, request);
    if (apdu_length != send)
    {
        PRINT("ERROR: couldn't send the request to write multiple registers\n");
        return -1;
    }

    // receives the response
    received = receive_modbus(socketfd, request);
    if (received < 0)
    {
        PRINT("ERROR: couldn't receive the response\n");
        return -1;
    }

    // checks if the response is valid
    if (request[0] & 0x80)
    {
        PRINT("ERROR: invalid response\n");
        return request[1];
    }
    return 0;
}
// reads the holding registers
// returns 0 if it was successful
// reads multiples registers in a TCP socket

int read_holding_registers(int socketfd, u_int16_t starting_address, u_int16_t quantity, u_int16_t *data_read)
{
    // checks all the function parameters
    if (quantity < MIN_REG_QUANTITY || quantity > MAX_REG_TO_READ_QUANTITY)
    {
        PRINT("ERROR: invalid quantity of registers\n");
        return -1;
    }
    // checks if the starting address is valid
    if (starting_address < MIN_REG_ADDRESS || starting_address > MAX_REG_ADDRESS)
    {
        PRINT("ERROR: invalid starting address\n");
        return -1;
    }
    if (starting_address + quantity > MAX_REG_ADDRESS)
    {
        PRINT("ERROR: invalid starting address\n");
        return -1;
    }
    if (socketfd < 0)
    {
        PRINT("ERROR: invalid socket\n");
        return -1;
    }
    if (data_read == NULL)
    {
        PRINT("ERROR: invalid data array\n");
        return -1;
    }
    // APDU length = 5 -> 1 byte for function code + 2 bytes for starting address + 2 bytes for quantity of registers
    int apdu_length = 5;
    int send = 0;
    int received = 0;

    uint8_t request[apdu_length];
    uint8_t response[apdu_length];

    // build the header
    request[0] = (uint8_t)FUNCTION_RMD;              // function code
    request[1] = (uint8_t)(starting_address >> 8);   // high byte of starting address
    request[2] = (uint8_t)(starting_address & 0xFF); // low byte starting address
    request[3] = (uint8_t)(quantity >> 8);           // high byte of quantity of registers
    request[4] = (uint8_t)(quantity & 0xFF);         // low byte of quantity of registers

    send = send_modbus(socketfd, apdu_length, request);

    // checks if there was no data lost in the sending
    if (apdu_length != send)
    {
        PRINT("ERROR: couldn't send the request to write multiple registers\n");
        return -1;
    }

    // receives the response
    received = receive_modbus(socketfd, request);
    // checks if there was no data lost in the receiving
    if (received < 0)
    {
        PRINT("ERROR: couldn't receive the response\n");
        return -1;
    }

    // checks if the response is valid
    if (request[0] & 0x80)
    {
        PRINT("ERROR: invalid response\n");
        return request[1];
    }

    // transfers the data from the response to the data array
    for (int i = 0; i < quantity; i++)
    {
        data_read[i] = (request[2 * i + 2] << 8) | request[2 * i + 3];
    }

    return 0;
}

#undef PRINT
#undef DEBUG
