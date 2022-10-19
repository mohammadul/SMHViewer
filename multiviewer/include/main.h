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
 * @file main.h
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2015-2022 Sk. Mohammadul Haque.
 * @brief This file contains declarations of main structures and functions.
 */

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glu.h>
#include <glstereo.h>
#include <meshlib.h>
#include <cfloat>
#include <cmath>
#include "../../mvutils/include/merror.h"
#include "../../mvutils/include/tinypng.h"
#include "../../mvutils/include/quat.hpp"
#include "../../mvutils/include/colours.h"
#include "common.h"
#include "../../mvutils/include/mvcanvas.h"
#include <cfgpreter.h>

typedef struct mesh_face_normal
{
    MESH_NORMAL cn;
} mesh_face_normal; /**< Mesh face normal structure */
typedef mesh_face_normal* MESH_FACE_NORMAL; /**< Pointer to mesh face normal structure */

/** \brief Extracts file name from full path
 *
 * \param[in] str Pointer to the input path
 * \return Pointer to the file name
 *
 */

char *extract_filename(char *str);

/** \brief Calculates mesh face normals
 *
 * \param[in] m Input mesh
 * \param[in] thrangdeg Threshold angle in degrees
 * \return Pointer to computed mesh face normal structure
 *
 */

MESH_FACE_NORMAL mesh_calc_face_normals2(MESH m, FLOATDATA thrangdeg);

/** \brief Frees the given indexed mesh face normals
 *
 * \param[in] num Input mesh index
 *
 */

void mesh_free_face_normals2(int num);

/** \brief Smoothes mesh face normals
 *
 * \param[in] m Input mesh
 * \param[in] mfc Pointer to given mesh face normal structure
 *
 */

void mesh_draw_mesh_smooth2(MESH m, MESH_FACE_NORMAL mfc);

/** \brief Initialises current mesh file
 *
 *
 */

void init_current_file();

/** \brief Sets window icon
 *
 * \param[in] w Pointer to SDL window
 *
 */

void set_icon(SDL_Window* w);

/** \brief Initialises font given a font name
 *
 * \param[in] fontname Given font name
 *
 */

void init_ttf(const char* fontname);

/** \brief Renders text at a given position
 *
 * \param[in] x X co-ordinate
 * \param[in] y Y co-ordinate
 *
 */

void render_text(int x, int y);

/** \brief Generates the 3-D axes
 *
 * \param[in] sc Scale of the axes
 *
 */

MESH generate_axes(FLOATDATA sc);
void get_exec_path(char* buffer);

/** \brief Initialises the overall system
 *
 *
 */

void init();

/** \brief Initialises the OpenGL system
 *
 *
 */

void initGL();

/** \brief Checks if pseudocolours generated
 *
 *
 */

void check_colour_gen();

/** \brief Swaps vertex colours with scalar colours
 *
 *
 */

void swap_vcolors_with_vscalars();

/** \brief Loads files
 *
 * \param[in] num Index of file to load
 *
 */

void load_files(int num);

/** \brief Loads given a configuration file name
 *
 * \param[in] fname Coniguration filename
 *
 */

void load_infra(const char* fname);

/** \brief Handles given a SDL key code
 *
 * \param[in] key SDL key code
 *
 */

void handle_keys(SDL_Keycode key);

/** \brief Projects a given point on S_2
 *
 * \param[in] r Radius
 * \param[in] x X co-ordinate
 * \param[in] y Y co-ordinate
 *
 */

FLOATDATA project(FLOATDATA r, FLOATDATA x, FLOATDATA y);

/** \brief Tracks pointer
 *
 * \param[in] type Tracking type
 * \param[in] x X co-ordinate
 * \param[in] y Y co-ordinate
 *
 */

void track_pointer(int type, int x, int y);

/** \brief Updates the SDL system
 *
 *
 */

void update();

/** \brief Updates the stereo system
 *
 * \param[in] baseline Stereo baseline
 * \param[in] refdepth Stereo reference plane
 * \param[in] fov Field of view in degrees
 * \param[in] nplane Near plane
 * \param[in] fplane Far plane
 * \param[in] aratio Aspect ratio
 * \param[in] type Streo type (GLSTEREO_RED_CYAN/GLSTEREO_GREEN_MAGENTA)
 *
 */

void glstereo_update(float baseline, float refdepth, float fov, float nplane, float fplane, float aratio, int type);

/** \brief Closes the SDL system
 *
 *
 */

void close();


#endif // MAIN_H_INCLUDED
