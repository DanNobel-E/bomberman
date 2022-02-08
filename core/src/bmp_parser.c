#include <SDL.h>
#include "bmp_parser.h"

Uint8 *bmp_open_file(const char *file_path, Sint64 *file_size)
{

    SDL_RWops *rw = SDL_RWFromFile(file_path, "rb");
   
    if (*file_size==0)
    {
        *file_size = SDL_RWsize(rw);
        return NULL;
    }

    Uint8 *content = SDL_malloc(*file_size);
    if (!content)
    {

        SDL_free(content);
        return NULL;
    }

    SDL_RWread(rw, content, *file_size, 1);
    SDL_RWclose(rw);

    return content;
}

Uint8 *bmp_parse(Uint8 *file_data, Uint32 *texture_width, Uint32 *texture_height)
{
    if (!file_data)
        return NULL;

    Uint32 bmp_file_size;
    SDL_memcpy(&bmp_file_size, file_data + 2, 4);

    Uint32 bmp_pixels_offset;
    SDL_memcpy(&bmp_pixels_offset, file_data + 10, 4);

    Uint32 image_size[2];
    SDL_memcpy(image_size, file_data + 18, 8);
    *texture_width = image_size[0];
    *texture_height = image_size[1];
    SDL_Log("The BMP size is %llux%llu", image_size[0], image_size[1]);

    Uint32 pixels_data_size;
    SDL_memcpy(&pixels_data_size, file_data + 34, 4);

    const Uint32 alignment = 4;
    Uint32 row_size = image_size[0] * 3;
    Uint32 padded_row_size = (row_size / alignment) * alignment;
    if (row_size % alignment != 0)
    {
        padded_row_size += alignment;
    }

    Uint32 full_padded_size = padded_row_size * image_size[1];
    Uint32 texture_size = row_size * image_size[1];

    Uint8 *texture_pixels = SDL_malloc(texture_size);

    if (!texture_pixels)
    {

        SDL_free(texture_pixels);
        return NULL;
    }

    for (Uint32 y = 0; y < image_size[1]; y++)
    {

        Uint8 *texture_row = texture_pixels + (row_size * y);
        Uint8 *bitmap_row = (file_data + bmp_pixels_offset) + (padded_row_size * ((image_size[1] - 1) - y)); // compute the address of row of the bitmap (they are inverted!!!)

        SDL_memcpy(texture_row, bitmap_row, row_size);
    }

    return texture_pixels;
}

SDL_Texture *BPM_Test()
{

    SDL_RWops *rw = SDL_RWFromFile("./Sprites/sample.bmp", "rb");

    Sint64 file_size = SDL_RWsize(rw);

    SDL_Log("The file size is %lld bytes", file_size);

    Uint8 *content = SDL_malloc(file_size);

    SDL_RWread(rw, content, file_size, 1);
    SDL_RWclose(rw);

    SDL_Log("First two chars: %c %c", content[0], content[1]);
    Uint32 bmp_file_size;
    SDL_memcpy(&bmp_file_size, content + 2, 4);

    SDL_Log("The BMP file size is %llu bytes", bmp_file_size);

    Uint32 bmp_pixels_offset;
    SDL_memcpy(&bmp_pixels_offset, content + 10, 4);

    SDL_Log("The BMP pixels are at offset %llu bytes", bmp_pixels_offset);

    Uint32 image_size[2];
    SDL_memcpy(image_size, content + 18, 8);

    SDL_Log("The BMP size is %llux%llu", image_size[0], image_size[1]);

    Uint32 pixels_data_size;
    SDL_memcpy(&pixels_data_size, content + 34, 4);

    SDL_Log("The BMP pixels data size is %llu", pixels_data_size);

    const Uint32 alignment = 4;
    Uint32 row_size = image_size[0] * 3;
    SDL_Log("A BMP row is %llu bytes", row_size);
    Uint32 padded_row_size = (row_size / alignment) * alignment;
    if (row_size % alignment != 0)
    {
        padded_row_size += alignment;
    }

    SDL_Log("A BMP padded row is %llu bytes", padded_row_size);

    Uint32 full_padded_size = padded_row_size * image_size[1];
    SDL_Log("A BMP padded pixel data is %llu bytes", full_padded_size);

    // TODO
    for (Uint32 y = 0; y < image_size[1]; y++)
    {
        Uint8 *texture_row; // compute the address of the texture row
        Uint8 *bitmap_row;  // compute the address of row of the bitmap (they are inverted!!!)
        SDL_memcpy(texture_row, bitmap_row, row_size);
    }

    return NULL;
}