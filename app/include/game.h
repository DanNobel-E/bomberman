#pragma once

#include <SDL.h>
#include <double_linked_list.h>
#include <double_list_item.h>
#include "bmb_bomberman.h"
#include "bmb_client_udp.h"
#include "png_parser.h"
#include "bmp_parser.h"
#include "bmb_timer.h"

void game_init(SDL_Window **window, SDL_Renderer **renderer, level_t *level, 
                player_item **players_ptr, int num_players, 
                texture_data_t **players_texture, socket_info_t *socket_info);

void game_run(SDL_Window **window, SDL_Renderer **renderer, level_t *level, 
                player_item **players_ptr, int num_players, 
                texture_data_t **players_texture, socket_info_t *socket_info);
                
void game_player_input(SDL_Event *event, movable_t * player_movable);

void game_quit(player_item **players_ptr, socket_info_t *socket_info);
