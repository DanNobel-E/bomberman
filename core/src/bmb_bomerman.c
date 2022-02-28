#include "bmb_bomberman.h"
#include "png_parser.h"
#include "bmp_parser.h"

void bmb_bomberman_init(bomberman_t *player, float x, float y, uint32_t w, uint32_t h, float speed, Uint8 *texture_data)
{
    player->movable.x = x;
    player->movable.y = y;
    player->movable.width = w;
    player->movable.height = h;
    player->movable.deltamove = (deltamove_t){0, 0, 0, 0};
    player->movable.speed = speed;
    player->player_rect = (SDL_Rect){0, 0, player->movable.width, player->movable.height};
    player->texture_data.pixels = png_parse(texture_data,
                                            &(player->texture_data.width),
                                            &(player->texture_data.height));

    // player->texture_data.pixels = bmp_parse(file_data,
    //                                 &player->texture_data.width,
    //                                 &player->texture_data.height);

    player->texture_data.texture_rect = (SDL_Rect){0, player->texture_data.height * 0.25f, player->texture_data.width, player->texture_data.height};

}