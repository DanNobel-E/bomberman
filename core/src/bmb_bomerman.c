#include "bmb_bomberman.h"
#include "png_parser.h"
#include "bmp_parser.h"

void bmb_bomberman_init(bomberman_t *player, float x, float y, uint32_t w, uint32_t h, float speed, Uint8 *texture_data, Uint8 *pixel_copy)
{
    player->movable.x = x;
    player->movable.y = y;
    player->movable.width = w;
    player->movable.height = h;
    player->movable.deltamove = (deltamove_t){0, 0, 0, 0};
    player->movable.speed = speed;
    player->player_rect = (SDL_Rect){player->movable.x, player->movable.y, player->movable.width, player->movable.height};
    if (!pixel_copy)
    {

        player->texture_data.pixels = png_parse(texture_data,
                                                &(player->texture_data.width),
                                                &(player->texture_data.height));

        // player->texture_data.pixels = bmp_parse(file_data,
        //                                 &player->texture_data.width,
        //                                 &player->texture_data.height);
      

    }
    else if(!texture_data)
    {
        player->texture_data.pixels = pixel_copy;
    }

    player->texture_data.texture_rect = (SDL_Rect){0, player->texture_data.height * 0.25f, player->texture_data.width, player->texture_data.height};
}

int bmb_bomberman_init_texture(bomberman_t *player, SDL_Renderer *renderer, uint32_t pixel_format, uint8_t channels)
{
    player->texture_data.texture = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STATIC, player->texture_data.width, player->texture_data.height);

    if (!player->texture_data.texture)
        return -1;

    SDL_SetTextureBlendMode(player->texture_data.texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(player->texture_data.texture, NULL, player->texture_data.pixels, player->texture_data.width * channels);

    return 0;
}