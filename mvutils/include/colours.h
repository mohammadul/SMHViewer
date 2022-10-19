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
 * @file colours.h
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2020-2022 Sk. Mohammadul Haque.
 * @brief This file contains declarations of colour structures and functions.
 */

#ifndef COLOURS_H_INCLUDED
#define COLOURS_H_INCLUDED

#include <cmath>
#include <limits>
#include <numeric>
#include <array>
#include <vector>

/** \brief Colour class
 *
 */

class Colour
{
public:
    using components = std::array<float,3>;
    Colour() : rgb{0.0,0.0,0.0} {}
    Colour(const components& c) : rgb(c) {}
    union {components rgb; components lab;};

    static constexpr components
    Black{0.0,0.0,0.0}, White{1.0,1.0,1.0}, Red{1.0,0.0,0.0},
          Green{0.0,1.0,0.0}, Blue{0.0,0.0,1.0}, Yellow{1.0,1.0,0.0},
          Magenta{1.0,0.0,1.0}, Cyan{0.0,1.0,1.0}, Orange{1.0,0.65,0.0},
          Gray{0.5,0.5,0.5}, BlueViolet{0.54,0.17,0.89}, Brown{0.65,0.16,0.16},
          DarkRed{0.55,0.0,0.0}, DarkGreen{0.0,0.39,0.0}, DarkBlue{0.0,0.0,0.55},
          DarkGray{0.66,0.66,0.66}, Maroon{0.69,0.19,0.38}, Pink{1.0,0.75,0.80},
          Purple{0.63,0.13,0.94}, FireBrick{0.7,0.13,0.13}, LightBlue{0.68,0.85,0.9},
          LightGray{0.83,0.83,0.83}, LightGreen{0.56,0.93,0.56}, RoyalBlue{0.25,0.41,0.88},
          Turquoise{0.25,0.88,0.82};
};

/** \brief Selects distinct colours
 *
 * \param[in] n Number of colours
 * \param[out] cols Selected colours
 *
 */

void distinctcolours(const int n, std::vector<Colour>&cols);


#endif // COLOURS_H_INCLUDED
