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
#include "bmb_timer.h"

#define PK_AUTH_ID 1
#define PK_COL_ID 2
#define PK_POS_ID 3




typedef struct socket_info
{

    int socket;
    struct sockaddr_in sin;
    bmb_timer_t timer;
    uint8_t auth;

} socket_info_t;

typedef struct packet_position
{

    uint8_t id;
    float x;
    float y;

} packet_position_t;



typedef struct packet_color
{

    uint8_t id;
    uint8_t r;
    uint8_t g;
    uint8_t b;


} packet_color_t;

typedef struct packet_auth
{

    uint8_t id;
    uint8_t auth;

} packet_auth_t;

int bmb_set_nb(int s);

int bmb_client_init(struct sockaddr_in *sin, int *s, const char *ip_address, const int port);

void bmb_client_send_packet(const struct sockaddr_in *sin, const int *s, const char *data, const int data_lenght);

packet_auth_t bmb_packet_auth(socket_info_t *socket_info);

int bmb_check_auth(socket_info_t *socket_info);

packet_color_t bmb_packet_color(uint8_t r, uint8_t g, uint8_t b);

int bmb_check_color(socket_info_t *socket_info, packet_color_t *packet_color);

packet_position_t bmb_packet_position(const float x, const float y);

void bmb_client_close(int *s);
