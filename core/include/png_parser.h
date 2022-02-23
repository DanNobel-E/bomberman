#include <SDL.h>
#include <zlib.h>

typedef struct png_chunk
{

    Uint32 length;
    char *type;
    Uint32 crc;
    Uint8 *data;
    Uint32 data_size;

} png_chunk_t;

typedef struct png_params
{

    Uint32 width;
    Uint32 height;
    Uint8 bit_depth;
    Uint8 color_type;
    Uint8 compression;
    Uint8 filter;
    Uint8 interlace;

} png_params_t;

/*
 *
 * On first call fills file_size with informations about memory to be allocated.
 * On second call returns a pointer to allocated memory with file datas.
 *@return a pointer to content data.
 *
 *@param file_path relative path of the file to open.
 *@param file_size size of the file content datas.
 *
 */
Uint8 *png_open_file(const char *file_path, Sint64 *file_size);

/*
 *
 * Parse raw bitmap datas into texture pixels datas.
 *@return a pointer to texture pixels data.
 *
 *@param file_data pointer to data content of a bitmap file.
 *@param texture_width where to store texture width infos.
 *@param texture_height where to store texture height infos.
 */
Uint8 *png_parse(Uint8 *file_data, Uint32 *texture_width, Uint32 *texture_height);

void png_init_chunk(png_chunk_t *chunk, const Uint32 len, char *type, const Uint32 crc, Uint8 *data, const Uint32 data_size);

int png_chunk_check(png_chunk_t **chunks_ptr, const int chunk_count, const char *chunk_name);

int png_IHDR_parse(Uint8 *IHDR_data, png_params_t *png_params, Uint32 *texture_width, Uint32 *texture_height);

void png_free_chunk(png_chunk_t *chunk);
