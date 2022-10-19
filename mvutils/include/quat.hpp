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
 * @file quat.hpp
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2015-2022 Sk. Mohammadul Haque.
 * @brief This file contains definitions of colors structures and functions.
 */

#ifndef QUAT_HPP_INCLUDED
#define QUAT_HPP_INCLUDED

#include <iostream>
#include <cmath>
#ifndef M_PI
#define M_PI (3.1415926535897932384626433)
#endif

template<typename T>
class quat
{
public:
    T x, y, z, w;
    quat():x(0), y(0), z(0), w(1) {}
    quat(T x_, T y_, T z_, T w_): x(x_), y(y_), z(z_), w(w_) {}
    template<typename U>
    quat(const quat<U>& q): x(q.x), y(q.y), z(q.z), w(q.w) {}

    quat<T> operator*(const quat<T>& rhs) const
    {
        return quat<T>(y*rhs.z- z*rhs.y+x*rhs.w+w*rhs.x,
            z*rhs.x-x*rhs.z+y*rhs.w+w*rhs.y,
            x*rhs.y-y*rhs.x+z*rhs.w+w*rhs.z,
            w*rhs.w-x*rhs.x-y*rhs.y-z*rhs.z);
    }

    void toangleaxis(T *a, T* ang) const
    {
        T s = std::sqrt(1.0-w*w);
        *ang = 2.0*acos(w);
        if(s>0)
        {
            s = 1.0/s;
            a[0] = s*x;
            a[1] = s*y;
            a[2] = s*z;
        }
        else
        {
            a[0] = 0;
            a[1] = 0;
            a[2] = 0;
        }
    }

    void fromangleaxis(const T x_, const T y_, const T z_, const T ang)
    {
        if(std::abs(ang)>0.0001)
        {
            T s = sin(ang/2.0);
            x = x_*s;
            y = y_*s;
            z = z_*s;
            w = cos(ang/2.0);
        }
        else
        {
            x = 0;
            y = 0;
            z = 0;
            w = 1.0;
        }
    }
};

#endif // QUAT_HPP_INCLUDED
