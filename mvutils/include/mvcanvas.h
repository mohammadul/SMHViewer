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
 * @file mvcanvas.cpp
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2020-2022 Sk. Mohammadul Haque.
 * @brief This file contains definitions of mvconvas structures and functions.
 */


#ifndef MVCANVAS_H
#define MVCANVAS_H

#include <vector>
#include <list>
#include "colours.h"
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <meshlib.h>

#define CURVE_MIN_DIST (1)
#define CURVE_MIN_DIST_SQR (CURVE_MIN_DIST*CURVE_MIN_DIST)

/** \brief Curve structure with vector
 *
 */

template <typename T>
struct curveV
{
    std::vector<std::pair<float,float>> points; /**< Point-set */
    Colour c; /**< Colour */
    float thickness; /**< Thickness */


    /** \brief Adds a given point to the curve.
     *
     * \param[in] x X co-ordinate
     * \param[in] y Y co-ordinate
     *
     */

    void add_point(float x, float y)
    {
        const auto& lst = points.back();
        if(std::abs(lst.first-x)>=CURVE_MIN_DIST || std::abs(lst.second-y)>=CURVE_MIN_DIST) points.push_back({x,y});
    }

    /** \brief Checks a given point if near to the curve.
     *
     * \param[in] x X co-ordinate
     * \param[in] y Y co-ordinate
     * \param[in] eta Distance
     *
     */

    bool is_near(float x, float y, float eta);

    /** \brief Clears all points from the curve.
     *
     *
     */

    void clear()
    {
        points.clear();
    }

    /** \brief Draws curve
     *
     *
     */

    void draw();

};


/** \brief Curve structure with list
 *
 */

struct curveL
{
    std::list<std::pair<float,float>> points; /**< Point-set */
    Colour c; /**< Colour */
    float thickness; /**< Thickness */

    /** \brief Adds a given point to the curve.
     *
     * \param[in] x X co-ordinate
     * \param[in] y Y co-ordinate
     *
     */

    void add_point(float x, float y)
    {
        const auto& lst = points.back();
        if(std::abs(lst.first-x)>=CURVE_MIN_DIST || std::abs(lst.second-y)>=CURVE_MIN_DIST) points.push_back({x,y});
    }

    /** \brief Checks a given point if near to the curve.
     *
     * \param[in] x X co-ordinate
     * \param[in] y Y co-ordinate
     * \param[in] eta Distance
     *
     */

    bool is_near(float x, float y, float eta);

    /** \brief Clears all points from the curve.
     *
     *
     */
    void clear()
    {
        points.clear();
    }

    /** \brief Draws curve
     *
     *
     */

    void draw();

};

using curve=curveL; /**< Curve structure */
// using curveSet = std::vector<curve>;
using curveSet = std::list<curve>; /**< Curveset structure */

extern std::vector<curveSet> curvesets;
extern curve currcurve;
extern int currcurveset;

extern float currcenterx, currcentery;
extern float currscale;
extern float dtheta;
extern float initheight;

/** \brief MVCanvas button structure
 *
 */

struct mvcanvas_button
{
public:
    int x, y;

private:
    float rad, radh;

public:
    Colour c;

    mvcanvas_button(): rad(0.0), radh(0.0) {}
    mvcanvas_button(int _x, int _y, float _rad, Colour _c):
        x(_x), y(_y), rad(_rad), radh(0.8*_rad), c(_c) {}

    int is_hit(int qx, int qy) const
    {
        return (((x-qx)*(x-qx)+(y-qy)*(y-qy))<=rad*rad);
    }
    void draw(int dx, int dy, bool selected);
};

extern bool enablecanvas;

/** \brief Initialises MVCanvas
 *
 *
 */

void mvcanvas_init();

/** \brief Processes MVCanvas key input
 *
 * \param[in] key Keycode
 *
 */

void mvcanvas_keyb(SDL_Keycode key);

/** \brief Processes MVCanvas mouse input
 *
 * \param[in] state Mouse state
 * \param[in] x Mouse position x
 * \param[in] y Mouse position y
 *
 */

void mvcanvas_mouse(int state, int x, int y);

/** \brief Processes MVCanvas motion
 *
 * \param[in] x Mouse position x
 * \param[in] y Mouse position y
 *
 */

void mvcanvas_motion(int x, int y);

/** \brief Processes MVCanvas resize input
 *
 * \param[in] w Width
 * \param[in] h Height
 *
 */

void mvcanvas_resize(int w, int h);

/** \brief Displays MVCanvas
 *
 *
 */

void mvcanvas_display();


#endif // MVCANVAS_H
