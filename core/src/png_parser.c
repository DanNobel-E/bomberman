#include <SDL.h>
#include <zlib.h>
#include <stdio.h>
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

    printf("%s\n", png_signature);
    // read chunks

    Uint32 chunk_len;
    SDL_memcpy(&chunk_len, file_data + 8, 4);
    chunk_len= _byteswap_ulong(chunk_len);   

    printf("%d\n", chunk_len);

    char chunk_type[4];
    SDL_memcpy(&chunk_type, file_data + 12, 4);
    

    printf("%s\n", chunk_type);

    Uint8 chunk_data[chunk_len];
    SDL_memcpy(chunk_data, file_data + 16, chunk_len);

    SDL_memcpy(texture_width,chunk_data,4);
    SDL_memcpy(texture_height,chunk_data+4,4);

    *texture_width= _byteswap_ulong(*texture_width);   
    *texture_height= _byteswap_ulong(*texture_height);   

    printf("w: %d h: %d\n", *texture_width,*texture_height);
    
    char bit_depth;
    SDL_memcpy(&bit_depth,chunk_data+8,1);

    char color_type;

    SDL_memcpy(&color_type,chunk_data+9,1);

    char compression_method;
    SDL_memcpy(&compression_method,chunk_data+10,1);

    char filter_method;
    SDL_memcpy(&filter_method,chunk_data+11,1);

    char interlace_method;
    SDL_memcpy(&interlace_method,chunk_data+12,1);

    printf("BD: %d CT: %d CM: %d FM: %d IM: %d\n", 
            bit_depth,color_type,compression_method,filter_method,interlace_method);



    
    int offset= 16+chunk_len;

    Uint32 chunk_crc;
    SDL_memcpy(&chunk_crc, file_data + offset, 4);
    chunk_crc= _byteswap_ulong(chunk_crc);   

    return NULL;
}
