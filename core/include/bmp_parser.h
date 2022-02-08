#include <SDL.h>

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
Uint8 *bmp_open_file(const char *file_path, Sint64 *file_size);

/*
 *
 * Parse raw bitmap datas into texture pixels datas.
 *@return a pointer to texture pixels data.
 *
 *@param file_data pointer to data content of a bitmap file.
 *
 */
Uint8 *bmp_parse(Uint8 *file_data, Uint32 *texture_width, Uint32 *  texture_height);
