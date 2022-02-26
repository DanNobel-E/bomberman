#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define PK_AUTH 1
#define PK_POS 2

typedef struct packet_position{

    float x;
    float y;
    char id;

}packet_position_t;

typedef struct packet_auth{

    char id;
    char auth;

}packet_auth_t;

int bmb_client_init(struct sockaddr_in * sin, int *s, const char* ip_address, const int port);

void bmb_client_send_packet(const struct sockaddr_in *sin, const int *s, const char *data, const int data_lenght);

void bmb_client_close(int *s);
