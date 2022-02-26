#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdint.h>

#define PK_AUTH_ID 1
#define PK_POS_ID 2

typedef struct packet_timer
{
    uint64_t current_tick;
    uint64_t prev_tick;
    uint64_t duration;
    double counter;
    

}packet_timer_t;

typedef struct socket_info
{

    int socket;
    struct sockaddr_in sin;
    packet_timer_t timer;

} socket_info_t;

typedef struct packet_position
{

    uint8_t id;
    float x;
    float y;

} packet_position_t;

typedef struct packet_auth
{

    uint8_t id;
    uint8_t auth;

} packet_auth_t;

int bmb_client_init(struct sockaddr_in *sin, int *s, const char *ip_address, const int port);

void bmb_client_send_packet(const struct sockaddr_in *sin, const int *s, const char *data, const int data_lenght);

packet_auth_t bmb_packet_auth();

packet_position_t bmb_packet_position(const float x, const float y);

void bmb_client_close(int *s);
