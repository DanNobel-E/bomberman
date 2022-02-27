#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include "bmb_client_udp.h"

int bmb_client_init(struct sockaddr_in *sin, int *s, const char *ip_address, const int port)
{

#ifdef _WIN32

    WSADATA wsa_data;
    if (WSAStartup(0x0202, &wsa_data))
    {

        printf("unable to initialize windosck2\n");
        return -1;
    }

#endif

    *s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*s < 0)
    {
        printf("unable to initialize the UDP socket\n");
        return -1;
    }

    printf("socket %d created\n", *s);

    inet_pton(AF_INET, ip_address, &sin->sin_addr);
    sin->sin_family = AF_INET;
    sin->sin_port = htons(port);

    return 0;
}

void bmb_client_send_packet(const struct sockaddr_in *sin, const int *s, const char *data, const int data_lenght)
{

    int sent_bytes = sendto(*s, data, data_lenght, 0, (struct sockaddr *)sin, sizeof(*sin));
    printf("sent %d bytes via UDP\n", sent_bytes);
}

packet_auth_t bmb_packet_auth(socket_info_t *socket_info)
{
    socket_info->auth = (uint8_t)rand();
    return (packet_auth_t){PK_AUTH_ID, socket_info->auth};
}

int bmb_check_auth(socket_info_t *socket_info)
{
    
    char buffer[2];
    int recv_bytes = recvfrom(socket_info->socket, buffer, 2, 0, (struct sockaddr*)&socket_info->sin,(int*)&socket_info->sin);

    socket_info->sin.sin_family = AF_INET;
    socket_info->sin.sin_port= htons(9999);

    if (recv_bytes > 0)
    {
        uint8_t id = buffer[0];
        uint8_t auth = buffer[1];

        if (id == PK_AUTH_ID)
        {

            if (auth == socket_info->auth)
                return 0;
        }
    }

    return -1;
}

packet_position_t bmb_packet_position(const float x, const float y)
{
    return (packet_position_t){PK_POS_ID, x, y};
}

void bmb_client_close(int *s)
{
    closesocket(*s);
}
