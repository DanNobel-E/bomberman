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

int bmb_set_nb(int s)
{
#ifdef _WIN32
    unsigned long nb_mode = 1;
    return ioctlsocket(s, FIONBIO, &nb_mode);
#else
    int flags = fcntl(s, F_GETFL, 0);
    if (flags < 0)
        return flags;
    flags |= O_NONBLOCK;
    return fcntl(s, F_SETFL, flags);
#endif
}

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

    bmb_set_nb(*s);
    printf("socket %d created\n", *s);

    inet_pton(AF_INET, ip_address, &sin->sin_addr);
    sin->sin_family = AF_INET;
    sin->sin_port = htons(port);

    return 0;
}

void bmb_client_send_packet(const struct sockaddr_in *sin, const int *s, const char *data, const int data_lenght)
{

    int sent_bytes = sendto(*s, data, data_lenght, 0, (struct sockaddr *)sin, sizeof(*sin));
}

packet_auth_t bmb_packet_auth(socket_info_t *socket_info)
{
    socket_info->auth = (uint8_t)rand();
    return (packet_auth_t){PK_AUTH_ID, socket_info->auth, 0};
}

packet_color_t bmb_packet_color(uint8_t r, uint8_t g, uint8_t b)
{
    return (packet_color_t){PK_COL_ID, r, g, b};
}

int bmb_check_color(socket_info_t *socket_info, packet_color_t *packet_color)
{

    char buffer[4];
    int recv_bytes = recv(socket_info->s, buffer, 4, 0);

    if (recv_bytes > 0)
    {

        uint8_t id = buffer[0];

        if (id == PK_COL_ID)
        {

            uint8_t r = buffer[1];
            uint8_t g = buffer[2];
            uint8_t b = buffer[3];
            *packet_color = bmb_packet_color(r, g, b);
            return 0;
        }
    }

    return -1;
}

int bmb_check_auth(bomberman_t *player, socket_info_t *socket_info)
{

    char buffer[3];
    int recv_bytes = recv(socket_info->s, buffer, 3, 0);

    if (recv_bytes > 0)
    {

        uint8_t id = buffer[0];

        if (id == PK_AUTH_ID)
        {

            uint8_t auth = buffer[1];

            if (auth == socket_info->auth)
            {
                uint8_t index = buffer[2];
                player->index = index;

                return 0;
            }
        }
    }

    return -1;
}

int bmb_check_new_player(socket_info_t *socket_info, player_item **players_ptr, SDL_Renderer *renderer, texture_data_t *players_texture)
{

    char buffer[16];
    int recv_bytes = recv(socket_info->s, buffer, 16, 0);

    if (recv_bytes > 0)
    {

        uint8_t id = buffer[0];

        if (id == PK_PLY_ID)
        {
            uint8_t r = buffer[1];
            uint8_t g = buffer[2];
            uint8_t b = buffer[3];
            uint8_t index = buffer[12];

            printf("received index: %d\n", index);

            float x = ((float *)buffer)[1];
            float y = ((float *)buffer)[2];

            bomberman_t *new_player = SDL_malloc(sizeof(bomberman_t));
            bmb_bomberman_init(new_player, x, y, 32, 32, 48, NULL, players_texture, 1);
            bmb_bomberman_set_color(new_player, r, g, b, 255);
            new_player->index = index;
            player_item *new_p_item = item_new(new_player, player_item);
            dlist_append(players_ptr, new_p_item, player_item);

            return 0;
        }
    }

    return -1;
}

packet_position_t bmb_packet_position(bomberman_t *player, const float x, const float y)
{
    return (packet_position_t){PK_POS_ID, player->index, x, y};
}

int bmb_check_position(socket_info_t *socket_info, player_item **players_ptr, bomberman_t *player)
{

    char buffer[12];
    int recv_bytes = recvfrom(socket_info->s, buffer, 12, 0, (struct sockaddr*)&socket_info->sin, (int*)&socket_info->sin);
            
    printf("recieved %d bytes via UDP\n", recv_bytes);

    if (recv_bytes > 0)
    {

        uint8_t id = buffer[0];

        if (id == PK_POS_ID)
        {
            uint8_t index = buffer[1];
            printf("updated player index: %d", index);

            if (index == player->index)
                return -1;

            for (int i = 0; i < dlist_count(players_ptr, player_item); i++)
            {

                bomberman_t *current_player = dlist_get_element_at(players_ptr, i, player_item)->object;
                if (current_player->index == index)
                {

                    float x = ((float *)buffer)[1];
                    float y = ((float *)buffer)[2];
                    current_player->movable.x = x;
                    current_player->movable.y = y;
                    current_player->player_rect.x = x;
                    current_player->player_rect.y = y;

                    return 0;
                }
            }
        }
    }

    return -1;
}

void bmb_client_close(int *s)
{
    closesocket(*s);
}
