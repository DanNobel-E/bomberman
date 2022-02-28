#pragma once

#include <SDL.h>
#include <stdint.h>

#define BLOCK_MASK_UNWALKABLE 0x0100
#define BLOCK_MASK_TELEPORT 0x0200
#define BLOCK_MASK_EXIT 0x0400

#define BLOCK_GROUND 0
#define BLOCK_WALL 1
#define BLOCK_DESTROYABLE 2

typedef struct level
{
    int32_t *cells;     // content of the level
    uint32_t cols;      // number of columns of the level grid
    uint32_t rows;      // number of rows of the level grid
    uint32_t cell_size; // size in pixel of a single cell
    SDL_Rect cell_rect;
} level_t;

typedef struct deltamove_t
{
    float right;
    float left;
    float down;
    float up;
} deltamove_t;
typedef struct movable
{
    float x;
    float y;
    uint32_t width;
    uint32_t height;
    deltamove_t deltamove;
    float speed;
} movable_t;

typedef struct texture_data
{
    SDL_Texture *texture;
    uint8_t *pixels;
    SDL_Rect texture_rect;
    uint32_t width;
    uint32_t height;
} texture_data_t;

typedef struct bomberman
{
    movable_t movable;
    uint32_t number_of_bombs;
    uint32_t dropped_bombs;
    uint32_t bomb_power;
    texture_data_t texture_data;
    SDL_Rect player_rect;
} bomberman_t;

/*
 *Check collision with level borders.
 *@return 0 value if collision happens.
 *
 *@param level_dim level checked dimension (width or height).
 *@param movable_dim movable dimension to check (width or height).
 *@param movable_coord pointer to movable coordinate to check and fix (x or y).
 *@param new_coord movable new coordinate to check for validation.
 */
int bmb_check_border_collisons(const uint32_t level_dim, const uint32_t movable_dim, float *movable_coord, const float new_coord);

/*
 *Moves a movable object on a given level.
 *@return index of the new cell the movable object has moved over. -1 if motion failed.
 *
 *@param level level where to move movable object.
 *@param movable movable object to move.
 *@param delta_x position variation on x axis.
 *@param delta_y position variation on y axis.
 */
int32_t bmb_move_on_level(level_t *level, movable_t *movable);

// initialize a level structure
int bmb_level_init(level_t *level, const uint32_t cols, const uint32_t rows, const uint32_t cell_size, int32_t *cells);
// get the cell content at the specified coordinates
int32_t bmb_level_cell(level_t *level, const uint32_t col, const uint32_t row);

void bmb_bomberman_init(bomberman_t *player, float x, float y, uint32_t w, uint32_t h, float speed, Uint8 *texture_data, Uint8 *pixel_copy);

int bmb_bomberman_init_texture(bomberman_t *player, SDL_Renderer *renderer, uint32_t pixel_format, uint8_t channels);
