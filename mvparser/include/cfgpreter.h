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
 * @file cfgpreter.h
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2019-2022 Sk. Mohammadul Haque.
 * @brief This file contains declarations of CFGPreter structures and functions.
 */

#ifndef CFGPRETER_H
#define CFGPRETER_H

#include <vector>
#include <cstdio>
#include <string>
#include <array>
#include <utility>


#define NUM_CFGWORDS (26)
#define CMD_ERROR (-1)
#define CMD_EOF (0)
#define CMD_PREV_MODEL (1)
#define CMD_NEXT_MODEL (2)
#define CMD_EXIT (3)
#define CMD_HEIGHT (4)
#define CMD_WIDTH (5)
#define CMD_SHININESS (6)
#define CMD_SPEC (7)
#define CMD_FOV (8)
#define CMD_PSIZE (9)
#define CMD_PCLOUD (10)
#define CMD_WIREFRAME (11)
#define CMD_AXIS (12)
#define CMD_LIGHTING (13)
#define CMD_VNORM (14)
#define CMD_FNORM (15)
#define CMD_SEMANTICS (16)
#define CMD_TRANSBG (17)
#define CMD_INFO (18)
#define CMD_RENDER_TYPE (19)
#define CMD_BG_COLOUR (20)
#define CMD_LT_COLOUR (21)
#define CMD_POS (22)
#define CMD_ROT (23)
#define CMD_SAVE (24)
#define CMD_TEXT (25)

#define TXTMAX_SZ (1024)

typedef FILE* FILEPOINTER;
#define MVCFG_RWRV_EOF (0)
#define MVCFG_RWRV_OKAY (1)
#define MVCFG_RWRV_COMMENT (2)

/** \brief SMHViewer configuration word structure
 *
 */

struct mvcfgword
{
    int codenum;
    std::vector<float> codevals;
    std::vector<std::string> codetxts;
    char iscomment;
};

/** \brief SMHViewer configuration structure
 *
 */

struct mvcfg
{
    int allocated;
    std::vector<mvcfgword> cwrds;
};

extern mvcfg cfgs; /**< SMHViewer configurations */
extern std::array<std::pair<std::string, std::pair<int, int>>,NUM_CFGWORDS> cfgstrings;  /**< SMHViewer configuration strings */

/** \brief Loads SMHViewer configurations into memory
 *
 * \param[in] fname Input filename
 *
 */

void mvcfg_load(const char *fname);

/** \brief Unloads SMHViewer configurations from memory
 *
 *
 */

void mvcfg_unload();

/** \brief Prints loaded SMHViewer configurations
 *
 *
 */

void mvcfg_print();

#endif // CFGPRETER_H
