#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>

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
    printf("sent %d bytes via UDP", sent_bytes);
}

void bmb_client_close(int *s)
{
    closesocket(*s);
}
