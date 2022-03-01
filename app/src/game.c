#include <SDL.h>
#include <time.h>
#include <stdio.h>
#include "game.h"
#include "bmb_level001.h"

#define CREATE_TXT_DATA 0
#define COPY_TXT_DATA 1

void game_init(SDL_Window **window, SDL_Renderer **renderer, level_t *level,
               player_item **players_ptr, int num_players,
               texture_data_t **players_texture, socket_info_t *socket_info)
{

    bmb_level_init(level, 8, 8, 64, level001_cells);
    level->cell_rect = (SDL_Rect){0, 0, level->cell_size, level->cell_size};

    SDL_Init(SDL_INIT_VIDEO);

    *window = SDL_CreateWindow("Bomberman",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               level->cols * level->cell_size,
                               level->rows * level->cell_size,
                               0);

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bomberman_t *player = (bomberman_t *)SDL_malloc(sizeof(bomberman_t));
    *players_ptr = item_new(player, player_item);

    *players_texture = SDL_malloc(sizeof(texture_data_t));
    Sint64 bmb_texture_size = 0;
    Uint8 channels = 0;
    Uint32 pixel_format = 0;

    png_open_file("./Sprites/Bomberman/Front/Bman_F_f00_pers.png", &bmb_texture_size);
    Uint8 *file_data = png_open_file("./Sprites/Bomberman/Front/Bman_F_f00_pers.png", &bmb_texture_size);
    channels = 4;
    pixel_format = SDL_PIXELFORMAT_RGBA32;

    //  bmp_open_file("./Sprites/ExplodableBlock.bmp", &bmb_texture_size);
    //  Uint8 *file_data = bmp_open_file("./Sprites/ExplodableBlock.bmp", &bmb_texture_size);
    //  channels=3;
    //  pixel_format= SDL_PIXELFORMAT_BGR24;

    bmb_bomberman_init(player, 100, 100, 32, 32, 48, file_data, *players_texture, CREATE_TXT_DATA);
    bmb_bomberman_init_texture(*players_texture, *renderer, pixel_format, channels);

    bmb_client_init(&socket_info->sin, &socket_info->s, "127.0.0.1", 9999);
    SDL_free(file_data);
}

void game_player_input(SDL_Event *event, movable_t *player_movable)
{

    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_RIGHT)
        {
            player_movable->deltamove.right = player_movable->speed * (1.0 / 60);
        }
        if (event->key.keysym.sym == SDLK_LEFT)
        {
            player_movable->deltamove.left = -player_movable->speed * (1.0 / 60);
        }
        if (event->key.keysym.sym == SDLK_DOWN)
        {
            player_movable->deltamove.down = player_movable->speed * (1.0 / 60);
        }
        if (event->key.keysym.sym == SDLK_UP)
        {
            player_movable->deltamove.up = -player_movable->speed * (1.0 / 60);
        }
    }
    if (event->type == SDL_KEYUP)
    {
        if (event->key.keysym.sym == SDLK_RIGHT)
        {
            player_movable->deltamove.right = 0;
        }
        if (event->key.keysym.sym == SDLK_LEFT)
        {
            player_movable->deltamove.left = 0;
        }
        if (event->key.keysym.sym == SDLK_DOWN)
        {
            player_movable->deltamove.down = 0;
        }
        if (event->key.keysym.sym == SDLK_UP)
        {
            player_movable->deltamove.up = 0;
        }
    }
}

void game_quit(player_item **players_ptr, socket_info_t *socket_info)
{

    player_item *player = dlist_get_element_at(players_ptr, 0, player_item);
    SDL_free(player->object->texture_data->pixels);
    SDL_free(player->object);
    dlist_destroy_item(&player, player_item);
    SDL_free(*players_ptr);
    bmb_client_close(&socket_info->s);
    SDL_Quit();
}

void game_run(SDL_Window **window, SDL_Renderer **renderer, level_t *level,
              player_item **players_ptr, int num_players,
              texture_data_t **players_texture, socket_info_t *socket_info)
{

    bomberman_t *player = dlist_get_element_at(players_ptr, 0, player_item)->object;

    // send authentication to server
    time_t seed = time(NULL);
    srand(seed);
    packet_auth_t packet_auth = bmb_packet_auth(socket_info);
    printf("id: %d auth: %d\n", packet_auth.id, packet_auth.auth);
    bmb_client_send_packet(&socket_info->sin, &socket_info->s, (char *)&packet_auth, sizeof(packet_auth_t));

    // wait for server response
    bmb_timer_t auth_check_timer;
    bmb_timer_start(&auth_check_timer, 1);

    while (bmb_check_auth(socket_info))
    {
        bmb_timer_tick(&auth_check_timer);
        if (!bmb_timer_stop(&auth_check_timer))
        {
            game_quit(players_ptr, socket_info);
            return;
        }
        bmb_timer_update(&auth_check_timer);
    }

    packet_color_t texture_color;
    // wait for color assignement
    while (bmb_check_color(socket_info, &texture_color))
    {
        bmb_timer_tick(&auth_check_timer);
        if (!bmb_timer_stop(&auth_check_timer))
        {
            game_quit(players_ptr, socket_info);
            return;
        }
        bmb_timer_update(&auth_check_timer);
    }
    SDL_SetTextureColorMod(player->texture_data->texture, texture_color.r, texture_color.g, texture_color.b);

    // set packet timer
    bmb_timer_start(&socket_info->timer, 1);

    int running = 1;
    while (running)
    {

        bmb_timer_tick(&socket_info->timer);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            // Input
            game_player_input(&event, &(player->movable));
        }

        SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 0);
        SDL_RenderClear(*renderer);

        for (uint32_t row = 0; row < level->rows; row++)
        {
            for (uint32_t col = 0; col < level->cols; col++)
            {
                int32_t cell = bmb_level_cell(level, col, row);
                int32_t cell_texture = cell & 0xff;
                level->cell_rect.x = col * level->cell_size;
                level->cell_rect.y = row * level->cell_size;

                if (cell_texture == BLOCK_GROUND)
                {
                    SDL_SetRenderDrawColor(*renderer, 0, 255, 0, 255);
                    SDL_RenderFillRect(*renderer, &level->cell_rect);
                }
                else if (cell_texture == BLOCK_WALL)
                {
                    SDL_SetRenderDrawColor(*renderer, 100, 50, 0, 255);
                    SDL_RenderFillRect(*renderer, &level->cell_rect);
                }
                else if (cell_texture == BLOCK_DESTROYABLE)
                {
                    SDL_SetRenderDrawColor(*renderer, 0, 50, 50, 255);
                    SDL_RenderFillRect(*renderer, &level->cell_rect);
                }
            }
        }

        // check for new players
        bmb_check_new_player(socket_info, players_ptr, *renderer, *players_texture);

        bmb_move_on_level(level, &player->movable);
        player->player_rect.x = player->movable.x;
        player->player_rect.y = player->movable.y;

        if (!bmb_timer_stop(&socket_info->timer))
        {
            packet_position_t packet_pos = bmb_packet_position(player->movable.x, player->movable.y);
            bmb_client_send_packet(&socket_info->sin, &socket_info->s, (char *)&packet_pos, sizeof(packet_position_t));
        }

        for (int i = 0; i < dlist_count(players_ptr, player_item); i++)
        {
            bomberman_t *p = dlist_get_element_at(players_ptr, i, player_item)->object;
            SDL_RenderCopy(*renderer, p->texture_data->texture, &p->texture_data->texture_rect, &p->player_rect);
        }

        SDL_RenderPresent(*renderer);

        bmb_timer_update(&socket_info->timer);
    }

    game_quit(players_ptr, socket_info);
}
