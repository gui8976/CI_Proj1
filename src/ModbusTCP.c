#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "modbusAPP.h"
#include <sys/time.h>
#include "modbusTCP.h"

// create a TCP socket

int TI = 0;
#define UNIT_ID 0x01;

// opens the socket
// defines a timeout
int open_socket()
{
    int sockets;
    sockets = socket(AF_INET, SOCK_STREAM, 0); // cria o pacote

    if (sockets < 0)
    {
        printf("ERROR: couldn't open socket, ending connection\n");
        return -1;
    }
    else
        printf("socket opened successfully\n");
    // sets the timeout
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // sets the timeout
    if (setsockopt(sockets, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        printf("ERROR: couldn't set the timeout\n");
        return -1;
    }
    else
        printf("timeout set successfully\n");

    return sockets;
}

// closes the socket
int close_socket(int sockets)
{
    return close(sockets);
}

// tries the connection to the server
// returns 0 if it was successful
int connect_socket_server(int sockets, char *ip_port, int port)
{

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_aton(ip_port, &server.sin_addr);

    if (connect(sockets, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("ERROR: couldn't connect to the server\n");
        return -1;
    }
    else
        printf("connected to the server\n");

    return 0;
}

int connect_to_ModbusTCP(int port, char *ip_port)
{
    int sockets = open_socket();
    if (sockets < 0)
    {
        printf("ERROR: couldn't open socket, ending connection\n");
        return -1;
    }
    else
        printf("socket opened successfully [CTCP]\n");

    if (connect_socket_server(sockets, ip_port, port) < 0)
    {
        printf("ERROR: couldn't connect to the server, ending connection\n");
        return -1;
    }
    else
        printf("connected to the server [CTCP]\n");

    return sockets;
}

// disconnects from the ModbusTCP server

int disconnect_from_ModbusTCP(int sockets)
{
    if (close_socket(sockets) < 0)
    {
        printf("ERROR: couldn't close socket, ending connection\n");
        return -1;
    }
    else
        printf("disconnected from the server successfully\n");
    return 0;
}

// sends a message to the server
int Send_Modbus_data(int sockets, uint8_t *response, int size_response)
{
    /*
    int length = send(sockets, response, size_response, 0);
    if (length < 0)
    {
        printf("ERROR: couldn't send data, ending connection\n");
        return -1;
    }
    else
        printf("Sent %d Bytes of Data", length);
    return length;
    */
    // checks how many bytes were sent
    int bytes_sent = 0;
    int bytes = 0;
    while (bytes_sent < size_response)
    {
        bytes = send(sockets, response + bytes_sent, size_response - bytes_sent, 0);
        if (bytes < 0)
        {
            printf("ERROR: couldn't send data, ending connection\n");
            return -1;
        }
        else
            printf("Sent %d Bytes of Data\n", bytes - 7);
        bytes_sent += bytes;
    }
    return bytes_sent;
}

// receives a message from the server
int Receive_Modbus_request(int sockets, uint8_t *request, int size_request)
{
    int length = 0;
    length = recv(sockets, request, size_request, 0);
    if (length < 0)
    {
        printf("ERROR: couldn't receive data, ending connection\n");
        return -1;
    }
    else
        printf("Received %d Bytes of Data\n", length);
    return length;
}

// sends a Modbus request to the server
// returns the size of the response if it was successful
// returns -1 if it wasn't successful

int send_Modbus(int sockets, int size_request, uint8_t *request)
{
    int TI = 0;
    // creates a MBAP header
    uint8_t MBAP_header[MAX_HEADER_SIZE];

    TI++;
    MBAP_header[0] = (uint8_t)(TI >> 8);
    MBAP_header[1] = (uint8_t)(TI & 0xFF);
    MBAP_header[2] = 0;
    MBAP_header[3] = 0;
    MBAP_header[4] = (uint8_t)(size_request + 1 >> 8);
    MBAP_header[5] = (uint8_t)(size_request + 1 & 0xFF);
    MBAP_header[6] = UNIT_ID;

    // creates a packet with the MBAP header and the request

    int size_packet = MAX_HEADER_SIZE + size_request;

    // allocates the memory for the packet
    uint8_t *packet = (uint8_t *)malloc(size_packet * sizeof(uint8_t));
    // checks if the memory was allocated
    if (packet == NULL)
    {
        printf("ERROR: couldn't allocate memory for the packet\n");
        return -1;
    }

    // copies the information to the packet
    memcpy(packet, MBAP_header, MAX_HEADER_SIZE);
    memcpy(packet + MAX_HEADER_SIZE, request, size_request);

    // prints the packet
    printf("Packet: ");
    for (int i = 0; i < size_packet; i++)
    {
        printf("%02X ", packet[i]);
    }
    printf("\n");
    printf("Packet size: %d\n", size_packet);

    // sends the packet
    int send = Send_Modbus_data(sockets, packet, size_packet);
    // checks if the packet was sent
    if (send < 0)
    {
        printf("ERROR: couldn't send the packet\n");
        return -1;
    }
    // prints the packet

    // frees the memory
    free(packet);
    // returns the number of data sent
    return send - MAX_HEADER_SIZE;
}

// receives a Modbus response from the server
// returns the size of the response if it was successful
// returns -1 if it wasn't successful

int receive_Modbus(int sockets, uint8_t *response)
{
    printf("Receiving Modbus response\n");
    // receive and analyze the MBAP header
    uint8_t MBAP_header[MAX_HEADER_SIZE];
    printf("Receiving MBAP header\n");

    int receive = Receive_Modbus_request(sockets, MBAP_header, MAX_HEADER_SIZE);
    // checks if the MBAP header was received
    if (receive < 0)
    {
        printf("ERROR: couldn't receive the MBAP header\n");
        return -1;
    }
    // prints the MBAP header
    printf("MBAP header: ");
    for (int i = 0; i < MAX_HEADER_SIZE; i++)
    {
        printf("%02X ", MBAP_header[i]);
    }
    printf("\n");

    // uses the MBAP header to allocate memory for the packet
    int size_packet = (MBAP_header[4] << 8) + MBAP_header[5] - 1;

    printf("Packet size: %d\n", size_packet);

    // allocates the memory for the packet
    uint8_t *packet = (uint8_t *)malloc(size_packet * sizeof(uint8_t));
    // checks if the memory was allocated
    if (packet == NULL)
    {
        printf("ERROR: couldn't allocate memory for the packet\n");
        return -1;
    }
    // receives the packet from the server
    printf("Receiving packet\n");
    receive = Receive_Modbus_request(sockets, packet, size_packet);
    // checks if the packet was received
    if (receive < 0)
    {
        printf("ERROR: couldn't receive the packet\n");
        return -1;
    }
    // prints the packet
    printf("Packet: ");
    for (int i = 0; i < size_packet; i++)
    {
        printf("%02X ", packet[i]);
    }
    printf("\n");
    printf("Packet size: %d\n", size_packet);
    // copies the information to the response
    memcpy(response,packet,size_packet);

    free(packet);
    return 0;
}
