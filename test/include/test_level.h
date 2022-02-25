#include "clove.h"
#include "bomberman.h"

CLOVE_TEST(LevelInitializationNULL)
{
    CLOVE_INT_EQ(bomberman_level_init(NULL, 0, 0, 0, NULL), -1);
}

CLOVE_TEST(LevelInitializationZeroCols)
{
    level_t level;
    CLOVE_INT_EQ(bomberman_level_init(&level, 0, 100, 100, NULL), -1);
}

CLOVE_TEST(LevelInitializationZeroRows)
{
    level_t level;
    CLOVE_INT_EQ(bomberman_level_init(&level, 100, 0, 100, NULL), -1);
}

CLOVE_TEST(LevelInitializationZeroSize)
{
    level_t level;
    CLOVE_INT_EQ(bomberman_level_init(&level, 100, 100, 0, NULL), -1);
}

CLOVE_TEST(LevelInitializationNullCells)
{
    level_t level;
    CLOVE_INT_EQ(bomberman_level_init(&level, 100, 100, 10, NULL), 0);
}

CLOVE_TEST(LevelCellsNULL)
{
    level_t level;
    bomberman_level_init(&level, 2, 2, 1, NULL);
    CLOVE_INT_EQ(bomberman_level_cell(&level, 0, 0), -1);
}

CLOVE_TEST(LevelCellsContent)
{
    level_t level;
    int32_t map[] = {1, 2, 3, 4};
    bomberman_level_init(&level, 2, 2, 1, map);
    CLOVE_INT_EQ(bomberman_level_cell(&level, 0, 0), 1);
}

CLOVE_TEST(LevelCellsOutOfBoundsCol)
{
    level_t level;
    int32_t map[] = {1, 2, 3, 4};
    bomberman_level_init(&level, 2, 2, 1, map);
    CLOVE_INT_EQ(bomberman_level_cell(&level, 3, 0), -1);
}

CLOVE_TEST(LevelCellsOutOfBoundsRow)
{
    level_t level;
    int32_t map[] = {1, 2, 3, 4};
    bomberman_level_init(&level, 2, 2, 1, map);
    CLOVE_INT_EQ(bomberman_level_cell(&level, 1, 5), -1);
}

CLOVE_TEST(LevelCellsOutOfBounds)
{
    level_t level;
    int32_t map[] = {1, 2, 3, 4};
    bomberman_level_init(&level, 2, 2, 1, map);
    CLOVE_INT_EQ(bomberman_level_cell(&level, 5, 5), -1);
}

CLOVE_SUITE(LevelSuite)
{
    CLOVE_SUITE_TESTS(
        LevelInitializationNULL,
        LevelInitializationZeroCols,
        LevelInitializationZeroRows,
        LevelInitializationZeroSize,
        LevelInitializationNullCells,
        LevelCellsNULL,
        LevelCellsContent,
        LevelCellsOutOfBoundsCol,
        LevelCellsOutOfBoundsRow,
        LevelCellsOutOfBounds
        );
}