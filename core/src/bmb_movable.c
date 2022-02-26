#include "bmb_bomberman.h"

int bmb_check_border_collisons(const uint32_t level_dim, const uint32_t movable_dim, float *movable_coord, const float new_coord)
{

    int result = 0;

    if (new_coord < 0)
        *movable_coord = 0;
    else if (new_coord + movable_dim >= level_dim)
        *movable_coord = level_dim - movable_dim;
    else
    {
        result -= 1;
    }

    return result;
}

int32_t bmb_move_on_level(level_t *level, movable_t *movable)
{
    float delta_x= movable->deltamove.right+movable->deltamove.left;
    float delta_y= movable->deltamove.up+movable->deltamove.down;


    if (delta_x == 0 && delta_y == 0)
        return -1;

    float new_x = movable->x + delta_x;
    float new_y = movable->y + delta_y;

    int32_t cell = -1;
    int dimension_offset = 0;
    int direction_offset = 1;

    if (delta_x != 0) // x axis movement
    {
        uint32_t level_width= level->cell_size*level->cols;
        if (!bmb_check_border_collisons(level_width, movable->width, &movable->x, new_x))
        {
            return -1;
        }

        if (new_x > movable->x)
        {
            dimension_offset = movable->width;
            direction_offset = 0;
        }

        uint32_t cell_x = (new_x + dimension_offset - 1) / level->cell_size;     // cell on which the movable will end
        uint32_t cell_y = (movable->y + movable->height - 1) / level->cell_size; // test for feet
        cell = bmb_level_cell(level, cell_x, cell_y);
        if (cell & BLOCK_MASK_UNWALKABLE) // collision!
        {
            movable->x = (cell_x + direction_offset) * level->cell_size - dimension_offset; // bring back
        }
        else
        {
            cell_y = movable->y / level->cell_size; // test for neck
            cell = bmb_level_cell(level, cell_x, cell_y);
            if (cell & BLOCK_MASK_UNWALKABLE) // collision!
            {
                movable->x = (cell_x + direction_offset) * level->cell_size - dimension_offset; // bring back
            }
            else
            {

                movable->x = new_x;
            }
        }
    }
    else // y axis movement
    {
        uint32_t level_height= level->cell_size*level->rows;

        if (!bmb_check_border_collisons(level_height, movable->height, &movable->y, new_y))
        {
            return -1;
        }

        if (new_y > movable->y)
        {
            dimension_offset = movable->height;
            direction_offset = 0;
        }

        uint32_t cell_x = (movable->x + movable->width - 1) / level->cell_size; // cell on which the movable will end
        uint32_t cell_y = (new_y + dimension_offset - 1) / level->cell_size;    // test for feet
        cell = bmb_level_cell(level, cell_x, cell_y);
        if (cell & BLOCK_MASK_UNWALKABLE) // collision!
        {
            movable->y = (cell_y + direction_offset) * level->cell_size - dimension_offset; // bring back
        }
        else
        {
            cell_x = movable->x / level->cell_size; // test for neck
            cell = bmb_level_cell(level, cell_x, cell_y);
            if (cell & BLOCK_MASK_UNWALKABLE) // collision!
            {
                movable->y = (cell_y + direction_offset) * level->cell_size - dimension_offset; // bring back
            }
            else
            {

                movable->y = new_y;
            }
        }
    }

    return cell;
}