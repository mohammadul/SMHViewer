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
 * @file tinypng.h
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2015-2022 Sk. Mohammadul Haque.
 * @brief This file contains declarations of tinypng structures and functions.
 */

#include <stdlib.h>
#include <stdint.h>
#include <png.h>
#include "merror.h"

#define TINY_PNG_RGB 0
#define TINY_PNG_RGB16 1
#define TINY_PNG_RGBA 2
#define TINY_PNG_RGBA16 3
#define TINY_PNG_GRAY 4
#define TINY_PNG_GRAY16 5

#define TINY_PNG_ORDER_DN 0
#define TINY_PNG_ORDER_UP 1

#define TINY_PNG_LITTLE_ENDIAN 0
#define TINY_PNG_BIG_ENDIAN 1

/** \brief Loads an image in PNG format
 *
 * \param[in] fname Input filename
 * \param[out] width Image width
 * \param[out] height Image height
 * \param[out] type Image type (TINY_PNG_RGB/TINY_PNG_RGB16/TINY_PNG_RGBA/TINY_PNG_RGBA16/TINY_PNG_GRAY/TINY_PNG_GRAY16)
 * \param[out] pixels Pixel data pointer
 * \param[in] order Pixel order (TINY_PNG_ORDER_DN/TINY_PNG_ORDER_UP)
 *
 */

void tiny_png_load(const char* fname, int* width, int* height, int* type, void** pixels, int order);

/** \brief Saves an image in PNG format
 *
 * \param[in] fname Input filename
 * \param[in] width Image width
 * \param[in] height Image height
 * \param[in] type Image type (TINY_PNG_RGB/TINY_PNG_RGB16/TINY_PNG_RGBA/TINY_PNG_RGBA16/TINY_PNG_GRAY/TINY_PNG_GRAY16)
 * \param[in] pixels Pixel data pointer
 * \param[in] order Pixel order (TINY_PNG_ORDER_DN/TINY_PNG_ORDER_UP)
 *
 */

void tiny_png_save(const char* fname, int width, int height, int type, void* pixels, int order);
