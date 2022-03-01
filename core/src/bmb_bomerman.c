#include <SDL.h>
#include "bmb_bomberman.h"
#include "png_parser.h"
#include "bmp_parser.h"

int bmb_bomberman_init(bomberman_t *player, float x, float y, uint32_t w, uint32_t h, float speed, Uint8 *file_data, texture_data_t *texture_data, int copy)
{
    player->movable.x = x;
    player->movable.y = y;
    player->movable.width = w;
    player->movable.height = h;
    player->movable.deltamove = (deltamove_t){0, 0, 0, 0};
    player->movable.speed = speed;
    player->player_rect = (SDL_Rect){player->movable.x, player->movable.y, player->movable.width, player->movable.height};

    if (!copy)
    {
        if (!file_data)
            return -1;

        texture_data->pixels = png_parse(file_data,
                                         &texture_data->width,
                                         &texture_data->height);

        // player->texture_data.pixels = bmp_parse(file_data,
        //                                 &player->texture_data.width,
        //                                 &player->texture_data.height);
        texture_data->texture_rect = (SDL_Rect){0, texture_data->height * 0.25f, texture_data->width, texture_data->height};
        player->texture_data = texture_data;
    }
    else
    {
        player->texture_data = texture_data;
    }

    return 0;
}

int bmb_bomberman_init_texture(texture_data_t *texture_data, SDL_Renderer *renderer, uint32_t pixel_format, uint8_t channels)
{
    texture_data->texture = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STATIC, texture_data->width, texture_data->height);

    if (!texture_data->texture)
        return -1;

    SDL_SetTextureBlendMode(texture_data->texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(texture_data->texture, NULL, texture_data->pixels, texture_data->width * channels);

    return 0;
}

void bmb_bomberman_set_color(bomberman_t *player, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{

    player->color.r = r;
    player->color.g = g;
    player->color.b = b;
    player->color.a = a;
}
