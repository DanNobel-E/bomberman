#pragma once

#include <SDL.h>
#include "bmb_bomberman.h"


void game_init(SDL_Window **window, SDL_Renderer **renderer, level_t *level, 
                bomberman_t **players_ptr, int num_players, 
                SDL_Texture **players_texture, socket_info_t *socket_info);

void game_run(SDL_Window **window, SDL_Renderer **renderer, level_t *level, 
                bomberman_t **players_ptr, int num_players, 
                SDL_Texture **players_texture, socket_info_t *socket_info);
                
void game_player_input(SDL_Event *event, movable_t * player_movable);