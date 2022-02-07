#include "bomberman.h"

int check_border_collisons(level_t *level, movable_t *movable, const float new_x, const float new_y)
{

    int result = 0;
    if (new_x < 0)
        movable->x = 0;
    else if (new_x + movable->width >= level->cols * level->cell_size)
        movable->x = (level->cols * level->cell_size) - movable->width;
    else
    {
        result = -1;
    }

    if (new_y < 0)
        movable->y = 0;
    else if (new_y + movable->height >= level->rows * level->cell_size)
        movable->y = (level->rows * level->cell_size) - movable->height;
    else
    {
        result = -1;
    }

    return result;
}

int32_t move_on_level(level_t *level, movable_t *movable, const float delta_x, const float delta_y)
{

    if (delta_x == 0 && delta_y == 0)
        return -1;

    float new_x = movable->x + delta_x;
    float new_y = movable->y + delta_y;

    if (!check_border_collisons(level, movable, new_x, new_y))
    {
        return -1;
    }

    int32_t cell = -1;
    int dimension_offset = 0;
    int direction_offset = 1;

    if (delta_x != 0) // x axis movement
    {

        if (new_x > movable->x)
        {
            dimension_offset = movable->width;
            direction_offset = 0;
        }

        uint32_t cell_x = (new_x + dimension_offset - 1) / level->cell_size;     // cell on which the movable will end
        uint32_t cell_y = (movable->y + movable->height - 1) / level->cell_size; // test for feet
        cell = level_cell(level, cell_x, cell_y);
        if (cell & BLOCK_MASK_UNWALKABLE) // collision!
        {
            movable->x = (cell_x + direction_offset) * level->cell_size - dimension_offset; // bring back
        }
        else
        {
            cell_y = movable->y / level->cell_size; // test for neck
            cell = level_cell(level, cell_x, cell_y);
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
        if (new_y > movable->y)
        {
            dimension_offset = movable->height;
            direction_offset = 0;
        }

        uint32_t cell_x = (movable->x + movable->width - 1) / level->cell_size; // cell on which the movable will end
        uint32_t cell_y = (new_y + dimension_offset - 1) / level->cell_size;    // test for feet
        cell = level_cell(level, cell_x, cell_y);
        if (cell & BLOCK_MASK_UNWALKABLE) // collision!
        {
            movable->y = (cell_y + direction_offset) * level->cell_size - dimension_offset; // bring back
        }
        else
        {
            cell_x = movable->x / level->cell_size; // test for neck
            cell = level_cell(level, cell_x, cell_y);
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