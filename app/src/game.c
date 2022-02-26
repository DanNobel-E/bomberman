#include <SDL.h>
#include <time.h>
#include <stdio.h>
#include "bmb_bomberman.h"
#include "bmb_level001.h"
#include "bmb_client_udp.h"
#include "png_parser.h"
#include "bmp_parser.h"

#define player(index) (*players_ptr)[index]

void game_init(SDL_Window **window, SDL_Renderer **renderer, level_t *level,
               bomberman_t **players_ptr, int num_players,
               SDL_Texture **players_texture, socket_info_t *socket_info)
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

    *players_ptr = (bomberman_t *)SDL_malloc(sizeof(bomberman_t) * num_players);

    Sint64 bmb_texture_size = 0;
    png_open_file("./Sprites/Bomberman/Front/Bman_F_f00_pers.png", &bmb_texture_size);
    Uint8 *file_data = png_open_file("./Sprites/Bomberman/Front/Bman_F_f00_pers.png", &bmb_texture_size);

    //  bmp_open_file("./Sprites/Bomberman/Front/bomberman.bmp", &bmb_texture_size);
    //  Uint8 *file_data = bmp_open_file("./Sprites/Bomberman/Front/bomberman.bmp", &bmb_texture_size);

    for (int i = 0; i < num_players; i++)
    {
        player(i).movable.x = 100;
        player(i).movable.y = 100;
        player(i).movable.width = 32;
        player(i).movable.height = 32;
        player(i).movable.deltamove = (deltamove_t){0, 0, 0, 0};
        player(i).movable.speed = 48;
        player(i).player_rect = (SDL_Rect){0, 0, player(i).movable.width, player(i).movable.height};
        player(i).texture_data.pixels = png_parse(file_data,
                                                  &(player(i).texture_data.width),
                                                  &(player(i).texture_data.height));
        player(i).texture_data.texture_rect = (SDL_Rect){0, player(i).texture_data.height * 0.25f, player(i).texture_data.width, player(i).texture_data.height};

        // (*players_ptr)[i].texture_data.pixels = bmp_parse(file_data,
        //                                                     &((*players_ptr)[i].texture_data.width),
        //                                                     &((*players_ptr)[i].texture_data.height));
    }

    *players_texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, player(0).texture_data.width, player(0).texture_data.height);
    SDL_SetTextureBlendMode(*players_texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(*players_texture, NULL, player(0).texture_data.pixels, player(0).texture_data.width * 4);
    SDL_free(file_data);

    bmb_client_init(&socket_info->sin, &socket_info->socket, "127.0.0.1", 9999);
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

void game_run(SDL_Window **window, SDL_Renderer **renderer, level_t *level,
              bomberman_t **players_ptr, int num_players,
              SDL_Texture **players_texture, socket_info_t *socket_info)
{

    socket_info->timer.prev_tick = SDL_GetPerformanceCounter();
    socket_info->timer.duration = 1;
    socket_info->timer.counter = socket_info->timer.duration;

    time_t seed = time(NULL);
    srand(seed);
    packet_auth_t packet = bmb_packet_auth();
    printf("id: %d auth: %d\n", packet.id, packet.auth);
    bmb_client_send_packet(&socket_info->sin, &socket_info->socket, (char *)&packet, sizeof(packet_auth_t));

    int running = 1;
    while (running)
    {

        socket_info->timer.current_tick = SDL_GetPerformanceCounter();
        socket_info->timer.counter -= (double)(socket_info->timer.current_tick - socket_info->timer.prev_tick) / SDL_GetPerformanceFrequency();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            // Input
            game_player_input(&event, &(player(0).movable));
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

        bmb_move_on_level(level, &player(0).movable);
        player(0).player_rect.x = player(0).movable.x;
        player(0).player_rect.y = player(0).movable.y;

        if (socket_info->timer.counter <= 0)
        {
            packet_position_t packet = bmb_packet_position(player(0).movable.x, player(0).movable.y);
            bmb_client_send_packet(&socket_info->sin, &socket_info->socket, (char *)&packet, sizeof(packet_position_t));
            socket_info->timer.counter = socket_info->timer.duration;
        }

        SDL_RenderCopy(*renderer, *players_texture, &player(0).texture_data.texture_rect, &player(0).player_rect);

        SDL_RenderPresent(*renderer);

        socket_info->timer.prev_tick= socket_info->timer.current_tick;
    }

    SDL_free(player(0).texture_data.pixels);
    bmb_client_close(&socket_info->socket);
    SDL_Quit();
}
