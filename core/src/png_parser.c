#include <SDL.h>
#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include "png_parser.h"

Uint8 *png_open_file(const char *file_path, Sint64 *file_size)
{

    SDL_RWops *rw = SDL_RWFromFile(file_path, "rb");

    if (*file_size == 0)
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

Uint8 *png_parse(Uint8 *file_data, Uint32 *texture_width, Uint32 *texture_height)
{

    if (!file_data)
        return NULL;

    const char *png_signature_std = "\x89PNG\r\n\x1a\n";
    char png_signature[8];
    SDL_memcpy(png_signature, file_data, 8);

    if (SDL_memcmp(png_signature_std, png_signature, 8) != 0)
        return NULL;

    // read chunks

    png_chunk_t *chunks = NULL;

    Uint32 offset = 8;
    const Uint8 len_size = 4;
    const Uint8 type_size = 4;
    Uint32 data_size = 0;
    const Uint8 crc_size = 4;
    Uint32 chunk_counter = 0;

    while (1)
    {
        // read chunk length
        Uint32 chunk_len;
        SDL_memcpy(&chunk_len, file_data + offset, len_size);
        chunk_len = _byteswap_ulong(chunk_len);
        data_size = chunk_len;
        offset += len_size;

        // read chunk type
        char chunk_type[type_size];
        SDL_memcpy(&chunk_type, file_data + offset, type_size);
        offset += type_size;

        // read chunk data
        Uint8 chunk_data[data_size];
        SDL_memcpy(chunk_data, file_data + offset, data_size);
        offset += data_size;

        Uint32 chunk_crc;
        SDL_memcpy(&chunk_crc, file_data + offset, crc_size);
        chunk_crc = _byteswap_ulong(chunk_crc);
        offset += crc_size;

        Uint32 type_crc = crc32(0, (const Bytef *)chunk_type, type_size);
        Uint32 checksum = crc32(type_crc, chunk_data, data_size);

        if (checksum != chunk_crc)
            return NULL;

        if (!chunks)
            chunks = (png_chunk_t *)SDL_malloc(sizeof(png_chunk_t));
        else
            chunks = (png_chunk_t *)SDL_realloc(chunks, sizeof(png_chunk_t) * (chunk_counter + 1));

        png_init_chunk(chunks + chunk_counter, chunk_len, chunk_type, chunk_crc, chunk_data, data_size);
        chunk_counter++;

        if (!SDL_memcmp(chunk_type, "IEND", type_size))
            break;
    }

    if (png_chunk_check(&chunks, chunk_counter, "IHDR") != 0)
        return NULL;

    png_params_t params;
    if (png_IHDR_parse(chunks[0].data, &params, texture_width, texture_height) != 0)
        return NULL;

    printf("w: %d h: %d\n", params.width, params.height);
    printf("BD: %d CT: %d CM: %d FM: %d IM: %d\n",
           params.bit_depth, params.color_type, params.compression, params.filter, params.interlace);

    if (png_chunk_check(&chunks, chunk_counter, "IDAT") != 0)
        return NULL;

    Uint8 *IDAT_total_data = NULL;
    int data_offset = 0;
    int data_index = 0;
    Uint32 total_data_size = 0;

    for (int i = 0; i < chunk_counter; i++)
    {
        if (!SDL_memcmp((chunks + i)->type, "IDAT", type_size))
        {

            total_data_size += (chunks + i)->data_size;
            if (!IDAT_total_data)
                IDAT_total_data = SDL_malloc(sizeof(Uint8) * total_data_size);
            else
                IDAT_total_data = SDL_realloc(IDAT_total_data, sizeof(Uint8) * total_data_size);

            SDL_memcpy(IDAT_total_data + data_offset, (chunks + i)->data, (chunks + i)->data_size);
            data_offset = total_data_size;
        }
    }

    uLongf uncompression_buffer_size = 1024;
    Uint8 *uncompressed_IDAT_data = (Uint8 *)SDL_malloc(sizeof(Uint8) * uncompression_buffer_size);

    int uncompression_result = uncompress(uncompressed_IDAT_data, &uncompression_buffer_size, IDAT_total_data, total_data_size);

    if (uncompression_result == Z_BUF_ERROR)
    {
        do
        {

            uncompression_buffer_size *= 2;
            uncompressed_IDAT_data = SDL_realloc((Uint8 *)uncompressed_IDAT_data, uncompression_buffer_size);
            uncompression_result = uncompress(uncompressed_IDAT_data, &uncompression_buffer_size, IDAT_total_data, total_data_size);

        } while (uncompression_result == Z_BUF_ERROR);

        if (uncompression_result == Z_MEM_ERROR || uncompression_result == Z_DATA_ERROR)
            return NULL;
    }
    else if (uncompression_result == Z_MEM_ERROR || uncompression_result == Z_DATA_ERROR)
        return NULL;

    Uint8 channels = 4;
    Uint32 stride = params.width * channels;

    if (uncompression_buffer_size != params.height * (stride + 1))
        return NULL;

    Uint8 *reconstructed_IDAT_data = SDL_malloc(sizeof(Uint8) * uncompression_buffer_size);

    int i = 0;
    for (int y = 0; y < params.height; y++)
    {
        Uint8 filter_type = uncompressed_IDAT_data[i];
        i++;
        for (int x = 0; x < stride; x++)
        {
            Uint8 current_byte = uncompressed_IDAT_data[i];
            i++;
            Uint8 filter;

            if (filter_type == 0)
                filter = 0;
            else if (filter_type == 1)
                filter = png_recon_a(reconstructed_IDAT_data, x, y, stride, channels);
            else if (filter_type == 2)
                filter = png_recon_b(reconstructed_IDAT_data, x, y, stride);
            else if (filter_type == 3)
            {
                Uint8 filter_a = png_recon_a(reconstructed_IDAT_data, x, y, stride, channels);
                Uint8 filter_b = png_recon_b(reconstructed_IDAT_data, x, y, stride);

                filter = floor((filter_a + filter_b) / 2);
            }
            else if (filter_type == 4)
            {
                Uint8 filter_a = png_recon_a(reconstructed_IDAT_data, x, y, stride, channels);
                Uint8 filter_b = png_recon_b(reconstructed_IDAT_data, x, y, stride);
                Uint8 filter_c = png_recon_c(reconstructed_IDAT_data, x, y, stride, channels);

                filter = filter_a + filter_b + filter_c;
            }
            else
            {

                return NULL;
            }

            reconstructed_IDAT_data[y*stride+x] = (current_byte + filter) & 0xff;
        }
    }

    SDL_free(IDAT_total_data);
    SDL_free(uncompressed_IDAT_data);
    for (int i = 0; i < chunk_counter; i++)
    {
        png_free_chunk(chunks + i);
    }
    chunks = NULL;

    return reconstructed_IDAT_data;
}

int png_chunk_check(png_chunk_t **chunks_ptr, const int chunk_count, const char *chunk_name)
{

    int result = -1;
    for (int i = 0; i < chunk_count; i++)
    {

        if (!SDL_memcmp((*chunks_ptr + i)->type, chunk_name, 4))
        {

            result = 0;
            break;
        }
    }

    return result;
}

void png_init_chunk(png_chunk_t *chunk, const Uint32 len, char *type, const Uint32 crc, Uint8 *data, const Uint32 data_size)
{

    chunk->length = len;
    chunk->type = (char *)SDL_malloc(sizeof(char) * 4);
    SDL_memcpy(chunk->type, type, 4);
    chunk->crc = crc;

    chunk->data = (Uint8 *)SDL_malloc(sizeof(Uint8) * data_size);
    SDL_memcpy(chunk->data, data, data_size);
    chunk->data_size = data_size;
}

int png_IHDR_parse(Uint8 *IHDR_data, png_params_t *params, Uint32 *texture_width, Uint32 *texture_height)
{

    Uint32 offset = 0;
    const Uint8 dim_size = 4;
    const Uint8 param_size = 1;

    SDL_memcpy(texture_width, IHDR_data + offset, dim_size);
    *texture_width = _byteswap_ulong(*texture_width);
    params->width = *texture_width;
    offset += dim_size;

    SDL_memcpy(texture_height, IHDR_data + offset, dim_size);
    *texture_height = _byteswap_ulong(*texture_height);
    params->height = *texture_height;
    offset += dim_size;

    if (params->width == 0 || params->height == 0)
        return -1;

    Uint8 bit_depth;
    SDL_memcpy(&bit_depth, IHDR_data + offset, param_size);
    params->bit_depth = bit_depth;
    offset += param_size;

    if (params->bit_depth != 8)
        return -1;

    Uint8 color_type;
    SDL_memcpy(&color_type, IHDR_data + offset, param_size);
    params->color_type = color_type;
    offset += param_size;

    if (params->color_type != 6)
        return -1;

    Uint8 compression_method;
    SDL_memcpy(&compression_method, IHDR_data + offset, param_size);
    params->compression = compression_method;
    offset += param_size;

    if (params->compression != 0)
        return -1;

    Uint8 filter_method;
    SDL_memcpy(&filter_method, IHDR_data + offset, param_size);
    params->filter = filter_method;
    offset += param_size;

    if (params->filter != 0)
        return -1;

    Uint8 interlace_method;
    SDL_memcpy(&interlace_method, IHDR_data + offset, param_size);
    params->interlace = interlace_method;
    offset += param_size;

    if (params->interlace != 0)
        return -1;

    return 0;
}

Uint8 png_paeth_predictor(const Uint8 a, const Uint8 b, const Uint8 c)
{
    Uint8 p = a + b - c;
    Uint8 pa = abs(p - a);
    Uint8 pb = abs(p - b);
    Uint8 pc = abs(p - c);

    Uint8 pr;
    if (pa <= pb && pa <= pc)
        pr = a;
    else if (pb <= pc)
        pr = b;
    else
        pr = c;

    return pr;
}

Uint8 png_recon_a(Uint8 *recon_data, const Uint8 x, const Uint8 y, const Uint32 stride, const Uint8 channels)
{
    if (x < channels)
        return 0;

    int offset = y * stride + x - channels;
    return *(recon_data + offset);
}

Uint8 png_recon_b(Uint8 *recon_data, const Uint8 x, const Uint8 y, const Uint32 stride)
{
    if (y <= 0)
        return 0;

    int offset = (y - 1) * stride + x;
    return *(recon_data + offset);
}

Uint8 png_recon_c(Uint8 *recon_data, const Uint8 x, const Uint8 y, const Uint32 stride, const Uint8 channels)
{
    if (y <= 0 || x < channels)
        return 0;

    int offset = (y - 1) * stride + x - channels;
    return *(recon_data + offset);
}

void png_free_chunk(png_chunk_t *chunk)
{
    SDL_free(chunk->type);
    SDL_free(chunk->data);
    chunk = NULL;
}
