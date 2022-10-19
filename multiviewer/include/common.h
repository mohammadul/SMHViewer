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
 * @file common.h
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2015-2022 Sk. Mohammadul Haque.
 * @brief This file contains declarations of common structures and functions.
 */

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define INIT_HEIGHT (480)
#define INIT_WIDTH (640)
#define INIT_FOV (48.0f)
#define INIT_SHININESS (50)
#define INIT_Z (-1.8f)
#define INIT_NEARPLANE (0.01f)
#define INIT_FARPLANE (20.0f)
#define INIT_BASELINE (0.05f)
#define INIT_REFDEPTH (-1.8f) //((INIT_NEARPLANE)+0.5f*(INIT_FARPLANE*INIT_NEARPLANE)/(INIT_FARPLANE-INIT_NEARPLANE))
#define INIT_STEREOTYPE (GLSTEREO_RED_CYAN)
#define INIT_TRACKBALLSIZE (0.6f)
#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define FNAME_PREFIX screenshot

#define INIT_PENTHICKNESS (2.0)

#if defined(_DEBUG)
#define DEBUG
#endif
#if defined(NDEBUG)
#undef DEBUG
#endif

#ifdef DEBUG
#define DEBUG_STDERR(x) {std::cerr<<(x);}
#define DEBUG_STDOUT(x) {std::cout<<(x);}
#else
#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
#endif
#define GLUT_DISABLE_ATEXIT_HACK
#include <meshlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

extern bool quit;

extern SDL_Window* gWindow;

extern void generate_text(MESH m, MESH_VECTOR3 mnm, MESH_VECTOR3 mxm);
extern int show_about(void* ptr);
extern bool text_ok;
extern bool show_minfo;
extern MESH* m1;
extern int curr_mesh;
extern GLfloat clear_col[4];
extern MESH_VECTOR3 minm, maxm;
extern bool needRedraw, recalcModelView;
extern bool wireframe;

extern SDL_Thread *thread_about;
extern bool sabout;

extern char folder_name[256];
extern int fcounter;
extern bool transpar;
extern void render();

extern int height, width;

#endif // COMMON_H_INCLUDED
