#include <SDL.h>
#include <stdio.h>
#include "bmb_bomberman.h"
#include "bmb_client_udp.h"
#include "bmp_parser.h"
#include "png_parser.h"
#include "game.h"

int main(int argc, char **argv)
{
    // init game
    SDL_Window *window;
    SDL_Renderer *renderer;
    level_t level001;
    bomberman_t *players;
    int num_players = 1;
    SDL_Texture *players_texture;

    game_init(&window, &renderer, &level001, &players, num_players, &players_texture);

    // Init client
    int socket;
    struct sockaddr_in sin;
    bmb_client_init(&sin, &socket, "127.0.0.1", 9999);

    game_run(&window, &renderer, &level001, &players, num_players, &players_texture);

    bmb_client_close(&socket);
    SDL_Quit();

    return 0;
}
