#include <SDL.h>
#include <stdio.h>
#include "double_list_item.h"
#include "bmb_bomberman.h"
#include "bmb_client_udp.h"
#include "game.h"


int main(int argc, char **argv)
{
    // init game
    SDL_Window *window;
    SDL_Renderer *renderer;
    level_t level001;
    player_item *players;
    int num_players = 1;
    texture_data_t *players_texture;
    socket_info_t socket_info;

    game_init(&window, &renderer, &level001, &players, num_players, &players_texture, &socket_info);
    game_run(&window, &renderer, &level001, &players, num_players, &players_texture, &socket_info);

    return 0;
}
