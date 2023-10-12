#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "ModbusAPP.h"
#include "ModbusTCP.h"
#include <sys/time.h>



#define DEBUG

#ifdef DEBUG

#define PRINT(...) printf(__VA_ARGS__)

#else 

#define PRINT(...)

#endif

// create a TCP socket_fd

// opens the socket_fd
// defines a timeout

int open_socket()
{
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0); // cria o pacote

    if (socket_fd < 0)
    {
        PRINT("ERROR: couldn't open socket_fd, ending connection\n");
        return -1;
    }
    else
        PRINT("socket_fd opened successfully\n");
    // sets the timeout
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // sets the timeout
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        PRINT("ERROR: couldn't set the timeout\n");
        return -1;
    }
    else
        PRINT("timeout set successfully\n");

    return socket_fd;
}

// closes the socket_fd
int close_socket(int socket_fd)
{
    return close(socket_fd);
}

// tries the connection to the server
// returns 0 if it was successful
int connect_socket_server(int socket_fd, char *ip_port, int port)
{

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_aton(ip_port, &server.sin_addr);

    if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        PRINT("ERROR: couldn't connect to the server\n");
        return -1;
    }
    else
        PRINT("connected to the server\n");

    return 0;
}

int connect_to_modbus_tcp(int port, char *ip_port)
{
    int socket_fd = open_socket();
    if (socket_fd < 0)
    {
        PRINT("ERROR: couldn't open socket_fd, ending connection\n");
        return -1;
    }
    else
        PRINT("socket_fd opened successfully [CTCP]\n");

    if (connect_socket_server(socket_fd, ip_port, port) < 0)
    {
        PRINT("ERROR: couldn't connect to the server, ending connection\n");
        return -1;
    }
    else
        PRINT("connected to the server [CTCP]\n");

    return socket_fd;
}

// disconnects from the ModbusTCP server

int disconnect_from_modbus_tcp(int socket_fd)
{
    if (close_socket(socket_fd) < 0)
    {
        PRINT("ERROR: couldn't close socket_fd, ending connection\n");
        return -1;
    }
    else
        PRINT("disconnected from the server successfully\n");
    return 0;
}

// sends a message to the server
int send_modbus_data(int socket_fd, uint8_t *response, int size_response)
{
    // checks how many bytes were sent
    int bytes_sent = 0;
    int bytes = 0;
    while (bytes_sent < size_response)
    {
        bytes = send(socket_fd, response + bytes_sent, size_response - bytes_sent, 0);
        if (bytes < 0)
        {
            PRINT("ERROR: couldn't send data, ending connection\n");
            return -1;
        }
        else
            PRINT("Sent %d Bytes of Data\n", bytes - 7);
        bytes_sent += bytes;
    }
    return bytes_sent;
}

// receives a message from the server
int receive_modbus_data(int socket_fd, uint8_t *request, int size_request)
{
    int length = 0;
    length = recv(socket_fd, request, size_request, 0);
    if (length < 0)
    {
        PRINT("ERROR: couldn't receive data, ending connection\n");
        return -1;
    }
    else
        PRINT("Received %d Bytes of Data\n", length);
    return length;
}

// sends a Modbus request to the server
// returns the size of the response if it was successful
// returns -1 if it wasn't successful

int send_modbus(int socket_fd, int size_request, uint8_t *request)
{
    static int id = 0;
    // creates a MBAP header
    uint8_t mbap_header[MAX_MBAP_HEADER_SIZE];
    uint16_t protocol_ID = 0x0000; // for modbus TCP/IP protocolID = 0

    id++;
    mbap_header[0] = (uint8_t)(id >> 8);
    mbap_header[1] = (uint8_t)(id & 0xFF);
    mbap_header[2] = (uint8_t)(protocol_ID >> 8);
    mbap_header[3] = (uint8_t)(protocol_ID & 0xFF);
    mbap_header[4] = (uint8_t)(size_request + 1 >> 8);
    mbap_header[5] = (uint8_t)(size_request + 1 & 0xFF);
    mbap_header[6] = UNIT_ID;

    // creates a packet with the MBAP header and the request
    int size_packet = MAX_MBAP_HEADER_SIZE + size_request;

    // allocates the memory for the packet
    uint8_t *packet = (uint8_t *)malloc(size_packet * sizeof(uint8_t));
    // checks if the memory was allocated
    if (packet == NULL)
    {
        PRINT("ERROR: couldn't allocate memory for the packet\n");
        return -1;
    }

    // copies the information to the packet
    memcpy(packet, mbap_header, MAX_MBAP_HEADER_SIZE);
    memcpy(packet + MAX_MBAP_HEADER_SIZE, request, size_request);

    // prints the packet
    PRINT("Packet: ");
    for (int i = 0; i < size_packet; i++)
    {
        PRINT("%02X ", packet[i]);
    }
    PRINT("\n");
    PRINT("Packet size: %d\n", size_packet);

    // sends the packet
    int send = send_modbus_data(socket_fd, packet, size_packet);
    // checks if the packet was sent
    if (send < 0)
    {
        PRINT("ERROR: couldn't send the packet\n");
        return -1;
    }

    // frees the memory
    free(packet);
    // returns the number of data sent
    return send - MAX_MBAP_HEADER_SIZE;
}

// receives a Modbus response from the server
// returns the size of the response if it was successful
// returns -1 if it wasn't successful

int receive_modbus(int socket_fd, uint8_t *response)
{
    PRINT("Receiving Modbus response\n");
    // receive and analyze the MBAP header
    uint8_t MBAP_header[MAX_MBAP_HEADER_SIZE];
    PRINT("Receiving MBAP header\n");

    int bytes_received = receive_modbus_data(socket_fd, MBAP_header, MAX_MBAP_HEADER_SIZE);
    // checks if the MBAP header was received
    if (bytes_received < 0)
    {
        PRINT("ERROR: couldn't receive the MBAP header\n");
        return -1;
    }
    // prints the MBAP header
    PRINT("MBAP header: ");
    for (int i = 0; i < MAX_MBAP_HEADER_SIZE; i++)
    {
        PRINT("%02X ", MBAP_header[i]);
    }
    PRINT("\n");

    // uses the MBAP header to allocate memory for the packet
    // subtracting 1 because the MBAP header already has the unit ID
    // index 4 and 5 are the size of the packet
    int packet_size = ((MBAP_header[4] << 8) | MBAP_header[5]) - 1;

    PRINT("Packet size: %d\n", packet_size);

    // allocates the memory for the packet
    uint8_t *packet = (uint8_t *)malloc(packet_size * sizeof(uint8_t));
    // checks if the memory was allocated
    if (packet == NULL)
    {
        PRINT("ERROR: couldn't allocate memory for the packet\n");
        return -1;
    }
    // receives the packet from the server
    PRINT("Receiving packet\n");
    bytes_received = receive_modbus_data(socket_fd, packet, packet_size);
    // checks if the packet was received
    if (bytes_received < 0 || bytes_received != packet_size)
    {
        PRINT("ERROR: couldn't receive the packet\n");
        return -1;
    }

    // prints the packet
    PRINT("Packet: ");
    for (int i = 0; i < packet_size; i++)
    {
        PRINT("%02X ", packet[i]);
    }

    PRINT("\n");
    PRINT("Packet size: %d\n", packet_size);
    // copies the information to the response
    memcpy(response, packet, packet_size);
    free(packet);

    return packet_size;
}


#undef PRINT
#undef DEBUG

