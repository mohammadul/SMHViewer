/**
    MIT License

    Copyright (c) 2022 Sk. Mohammadul Haque

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/**
 * @file tinypng.cpp
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2015-2022 Sk. Mohammadul Haque.
 * @brief This file contains definitions of tinypng structures and functions.
 */

#include "../include/tinypng.h"

/**< \cond */
int __tiny_png_check_endian()
{
    unsigned int i = 1;
    char *c = (char*)&i;
    if(*c) return TINY_PNG_LITTLE_ENDIAN;
    return TINY_PNG_BIG_ENDIAN;
}

int __tiny_png_setjmp(png_structp pptr)
{
    return setjmp(png_jmpbuf(pptr));
}

/**< \endcond */

void tiny_png_save(const char* fname, int width, int height, int type, void* pixels, int order)
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte color_type;
    png_byte bit_depth;
    int pitch, i;
    png_bytep* row_pointers;
    FILE *fp;
    if((fp = fopen(fname, "wb"))==NULL) warning("Cannot open PNG file.");
    if((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))==NULL)
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }

    if((info_ptr = png_create_info_struct(png_ptr))==NULL)
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }

    png_init_io(png_ptr, fp);

    switch(type)
    {
    case TINY_PNG_RGB:
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 8;
        pitch = 3*width;
        break;
    case TINY_PNG_RGB16:
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 16;
        pitch = 6*width;
        break;
    case TINY_PNG_RGBA:
        color_type = PNG_COLOR_TYPE_RGBA;
        bit_depth = 8;
        pitch = 4*width;
        break;
    case TINY_PNG_RGBA16:
        color_type = PNG_COLOR_TYPE_RGBA;
        bit_depth = 16;
        pitch = 8*width;
        break;
    case TINY_PNG_GRAY:
        color_type = PNG_COLOR_TYPE_GRAY;
        bit_depth = 8;
        pitch = width;
        break;
    case TINY_PNG_GRAY16:
        color_type = PNG_COLOR_TYPE_GRAY;
        bit_depth = 16;
        pitch = 2*width;
        break;
    default:
        warning("Cannot open PNG file.");
        color_type = PNG_COLOR_TYPE_RGB;
        bit_depth = 8;
        pitch = 3*width;
    }
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    if(__tiny_png_check_endian()==TINY_PNG_LITTLE_ENDIAN) png_set_swap(png_ptr);
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }
    if((row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*height))==NULL)
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }
    if(order==TINY_PNG_ORDER_DN) for(i=0; i<height; ++i) row_pointers[i] = (png_bytep)(uint8_t*)pixels+i*pitch;
    else for(i=0; i<height; ++i) row_pointers[height-i-1] = (png_bytep)(uint8_t*)pixels+i*pitch;
    png_write_image(png_ptr, row_pointers);
    free(row_pointers);
    if(__tiny_png_setjmp(png_ptr))
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }
    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

void tiny_png_load(const char *fname, int* width, int* height, int* type, void** pixels, int order)
{
    FILE* fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int i, pitch;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep* row_pointers;

    if((fp = fopen(fname, "rb"))==NULL) warning("Cannot open PNG file.");
    if((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))==NULL)
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }
    if((info_ptr = png_create_info_struct(png_ptr))==NULL)
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }

    if(__tiny_png_setjmp(png_ptr))
    {
        #if !defined(__linux__) || !defined(__linux)
        //png_read_destroy(png_ptr, info_ptr, (png_info *)0);
        #endif
        fclose(fp);
        warning("Cannot open PNG file.");
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    *width = png_get_image_width(png_ptr, info_ptr);
    *height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if(color_type==PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    #if !defined(__linux__) || !defined(__linux)
    //if(info_ptr->bit_depth<8) png_set_packing(png_ptr);
    #endif
    if(__tiny_png_check_endian()==TINY_PNG_LITTLE_ENDIAN) png_set_swap(png_ptr);

    png_start_read_image(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if((row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*(*height)))==NULL)
    {
        fclose(fp);
        warning("Cannot open PNG file.");
    }
    pitch = png_get_rowbytes(png_ptr,info_ptr);
    switch(color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
        if(bit_depth==8) *type = TINY_PNG_GRAY;
        else if(bit_depth==16) *type = TINY_PNG_GRAY16;
        break;
    case PNG_COLOR_TYPE_RGB:
        if(bit_depth==8) *type = TINY_PNG_RGB;
        else if(bit_depth==16) *type = TINY_PNG_RGB16;
        break;
    case PNG_COLOR_TYPE_RGBA:
        if(bit_depth==8) *type = TINY_PNG_RGBA;
        else if(bit_depth==16) *type = TINY_PNG_RGBA16;
        break;
    default:
        warning("Cannot open PNG file.");
    }

    if((row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*(*height)))==NULL) warning("Cannot open PNG file.");
    if((*pixels = malloc(pitch*(*height)))==NULL) warning("Cannot open PNG file.");
    if(order==TINY_PNG_ORDER_DN) for(i=0; i<(*height); ++i) row_pointers[i] = (png_bytep)(uint8_t*)(*pixels)+i*pitch;
    else for(i=0; i<(*height); ++i) row_pointers[(*height)-i-1] = (png_bytep)(uint8_t*)(*pixels)+i*pitch;
    png_read_image(png_ptr, row_pointers);
    #if !defined(__linux__) || !defined(__linux)
    //png_read_destroy(png_ptr, info_ptr, (png_infop)0);
    #endif
    free(png_ptr);
    free(row_pointers);
    free(info_ptr);
    fclose(fp);
}

