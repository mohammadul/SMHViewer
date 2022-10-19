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
 * @brief This file contains definitions of mvcanvas structures and functions.
 */

#include "../include/common.h"
#include "../../mvutils/include/mvcanvas.h"
#include "../../mvutils/include/tinypng.h"
#include <iostream>
std::vector<curveSet> curvesets(1);
curve currcurve;
int currcurveset = 0;

float currcenterx, currcentery;
float currscale = 1.0;
float dtheta = MESH_PI/30.0;
float initheight;
GLfloat currpenthickness = INIT_PENTHICKNESS;

#define NUM_BTNS (8+1)
mvcanvas_button btn[NUM_BTNS];
Colour selectedcolour;
int selectedbtn = 2;
bool enablecolourmenu = true;
bool enableeraser = false;
bool enablecanvas = false;

bool curve::is_near(float x, float y, float eta)
{
    float eta2 = eta*eta;
    if(points.size()==1)
    {
        /*
        if(((x-points[0].first)*(x-points[0].first)+(y-points[0].second)*(y-points[0].second))<=eta2)
        {
            return true;
        }
        */
        const auto felem = points.front();
        if(((x-felem.first)*(x-felem.first)+(y-felem.second)*(y-felem.second))<=eta2)
        {
            return true;
        }
    }
    else if(points.size()>0)
    {
        // for(std::size_t i=0; i<(points.size()-1); ++i)
        // {
        //      auto& cp1 = points[i], cp2 = points[i+1];
        for(auto it=points.begin(); std::next(it)!=points.end(); ++it)
        {
            auto& cp1 = *it;
            auto& cp2 = *(std::next(it));
            float vx = cp2.first-cp1.first, vy = cp2.second-cp1.second;
            float wx = x-cp1.first, wy = y-cp1.second;
            float c1 = vx*wx+vy*wy;
            float c2= vx*vx+vy*vy;
            if(c1<=0.0)
            {
                if((wx*wx+wy*wy)<=eta2)
                {
                    return true;
                }
            }
            else if(c2<=c1)
            {
                float qx = x-cp2.first, qy = y-cp2.second;
                if((qx*qx+qy*qy)<=eta2)
                {
                    return true;
                }
            }
            else
            {
                float rx = x-cp1.first-c1*vx/c2, ry = y-cp1.second-c1*vy/c2;
                if((rx*rx+ry*ry)<=eta2)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void curve::draw()
{
    glLineWidth(thickness);
    glColor3f(c.rgb[0], c.rgb[1], c.rgb[2]);
    if(points.size()==1)
    {
        float psz;
        glGetFloatv(GL_POINT_SIZE, &psz);
        glBegin(GL_POINT);
        glPointSize(thickness);
        // glVertex2i(points[0].first*currscale, points[0].second*currscale);
        const auto felem = points.front();
        glVertex2i(felem.first*currscale, felem.second*currscale);
        glEnd();
        glPointSize(psz);
    }
    else
    {
        glBegin(GL_LINE_STRIP);
        // for(size_t j=0; j<points.size(); ++j)
        // {
        //     glVertex2i(points[j].first*currscale, points[j].second*currscale);
        // }
        for(auto it=points.begin(); it!=points.end(); ++it)
        {
            glVertex2i(it->first*currscale, it->second*currscale);
        }
        glEnd();
    }
}

void mvcanvas_button::draw(int dx, int dy, bool selected)
{
    if(selected)
    {
        glColor3f(c.rgb[0], c.rgb[1], c.rgb[2]);
        glBegin(GL_POLYGON);
        for(float theta =-MESH_PI; theta<=MESH_PI; theta+=dtheta)
        {
            glVertex2f(dx+rad*float(std::cos(theta)),
                       dy+rad*float(std::sin(theta)));
        }
        glEnd();
        glLineWidth(rad*0.1);
        glColor3f(1.-c.rgb[0], 1.-c.rgb[1], 1.-c.rgb[2]);
        glBegin(GL_LINE_LOOP);
        for(float theta =-MESH_PI; theta<=MESH_PI; theta+=dtheta)
        {
            glVertex2f(dx+radh*float(std::cos(theta)),
                       dy+radh*float(std::sin(theta)));
        }
        glEnd();
        glLineWidth(currpenthickness);
    }
    else
    {
        glColor3f(c.rgb[0], c.rgb[1], c.rgb[2]);
        glBegin(GL_POLYGON);
        for(float theta =-MESH_PI; theta<=MESH_PI; theta+=dtheta)
        {
            glVertex2f(dx+rad*float(std::cos(theta)),
                       dy+rad*float(std::sin(theta)));
        }
        glEnd();
    }
}

void mvcanvas_init()
{
    selectedcolour = Colour::White;
    selectedbtn = 2;
    curvesets.resize(1);
    currcurve.c = selectedcolour;
    btn[1] = mvcanvas_button(15,15,15, Colour::Black);
    btn[2] = mvcanvas_button(45,15,15, Colour::White);
    btn[3] = mvcanvas_button(75,15,15, Colour::Red);
    btn[4] = mvcanvas_button(105,15,15, Colour::Green);
    btn[5] = mvcanvas_button(135,15,15, Colour::Blue);
    btn[6] = mvcanvas_button(165,15,15, Colour::Yellow);
    btn[7] = mvcanvas_button(195,15,15, Colour::Magenta);
    btn[8] = mvcanvas_button(225,15,15, Colour::Cyan);
    currcenterx = INIT_WIDTH/2.0;
	currcentery = INIT_HEIGHT/2.0;
	currscale = 1.0;
	currpenthickness = INIT_PENTHICKNESS;
	initheight = INIT_HEIGHT;
}


void mvcanvas_keyb(SDL_Keycode key)
{
    switch(key)
    {
    case SDLK_q:
        quit = true;
        break;
    case SDLK_c:
        curvesets[currcurveset].clear();
        currcurve.points.clear();
        break;
    case SDLK_p:
        enablecolourmenu = !enablecolourmenu;
        break;
    case SDLK_e:
        enableeraser = !enableeraser;
        break;
    case SDLK_1:
        selectedcolour = btn[1].c;
        selectedbtn = 1;
        break;
    case SDLK_2:
        selectedcolour = btn[2].c;
        selectedbtn = 2;
        break;
    case SDLK_3:
        selectedcolour = btn[3].c;
        selectedbtn = 3;
        break;
    case SDLK_4:
        selectedcolour = btn[4].c;
        selectedbtn = 4;
        break;
    case SDLK_5:
        selectedcolour = btn[5].c;
        selectedbtn = 5;
        break;
    case SDLK_6:
        selectedcolour = btn[6].c;
        selectedbtn = 6;
        break;
    case SDLK_7:
        selectedcolour = btn[7].c;
        selectedbtn = 7;
        break;
    case SDLK_8:
        selectedcolour = btn[8].c;
        selectedbtn = 8;
        break;
    case SDLK_HOME:
        enablecanvas = !enablecanvas;
        break;

    case SDLK_r:
        clear_col[0] +=0.05;
        if(clear_col[0]>1.0) clear_col[0] = 1.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        break;
    case SDLK_g:
        clear_col[1] +=0.05;
        if(clear_col[1]>1.0) clear_col[1] = 1.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        break;
    case SDLK_b:
        clear_col[2] +=0.05;
        if(clear_col[2]>1.0) clear_col[2] = 1.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        break;
    case SDLK_t:
        clear_col[0] -=0.05;
        if(clear_col[0]<0.0) clear_col[0] = 0.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        break;
    case SDLK_h:
        clear_col[1] -=0.05;
        if(clear_col[1]<0.0) clear_col[1] = 0.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        break;
    case SDLK_n:
        clear_col[2] -=0.05;
        if(clear_col[2]<0.0) clear_col[2] = 0.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        break;
    case SDLK_KP_PLUS:
        ++currpenthickness;
        if(currpenthickness>10.0) currpenthickness = 10.0;
        break;
    case SDLK_KP_MINUS:
        --currpenthickness;
        if(currpenthickness<1.0) currpenthickness = 1.0;
        break;
    case SDLK_F11:
        if(SDL_GetWindowFlags(gWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP) SDL_SetWindowFullscreen(gWindow, 0);
        else SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        recalcModelView = true;
        break;
    case SDLK_F1:
        if(!sabout)
        {
            thread_about = SDL_CreateThread(show_about, "About SMHViewer ModelViewer", (void *)NULL);
        }
        break;
    case SDLK_m:
        char fname[256];
        unsigned char *pixels;
        render(); /* force do render before capture */
        if((pixels = (unsigned char*) malloc(width*height*4))==NULL) warning("Cannot allocate memory for screenshot.");
        else
        {
#if defined(__WIN32) || defined(__WIN32__) ||defined(WIN32) || defined(WINNT)
            sprintf(fname, "%s" STR(FNAME_PREFIX) "%d.png", folder_name, fcounter);
#else
            sprintf(fname, "%s" STR(FNAME_PREFIX) "%d.png", folder_name, fcounter);
#endif
            ++fcounter;
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            tiny_png_save(fname, width, height, TINY_PNG_RGBA, pixels, TINY_PNG_ORDER_UP);
            free(pixels);
        }
        break;
    case SDLK_SEMICOLON:
        transpar = !transpar;
        if(transpar) clear_col[3] = 0.0;
        else clear_col[3] = 1.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        needRedraw = true;
        break;
    case SDLK_RIGHT:
        if((currcurveset+1)<curvesets.size()) ++currcurveset;
        needRedraw = true;
        break;
    case SDLK_LEFT:
        if(currcurveset>0) --currcurveset;
        needRedraw = true;
        break;
    case SDLK_i:
        curvesets.insert(curvesets.cbegin()+currcurveset+1, curveSet{});
        ++currcurveset;
        needRedraw = true;
        break;
    case SDLK_d:
        if(curvesets.size()>1)
        {
            if((currcurveset+1)>=curvesets.size())
            {
                --currcurveset;
                curvesets.erase(curvesets.cbegin()+currcurveset+1);
            }
            else
            {
                curvesets.erase(curvesets.cbegin()+currcurveset);
            }
            needRedraw = true;
        }
        break;
    }
}

bool wasdown = false;

void mvcanvas_mouse(int state, int x, int y)
{
    float sx = (x-currcenterx)/currscale;
    float sy = (y-currcentery)/currscale;
    if(state==SDL_MOUSEBUTTONUP && wasdown)
    {
        curvesets[currcurveset].emplace_back(currcurve);
        currcurve.clear();
        wasdown = false;
    }
    else if(state==SDL_MOUSEBUTTONUP)
    {
        wasdown = false;
    }
    if(state==SDL_MOUSEBUTTONDOWN && !wasdown)
    {
        wasdown = true;
        if(enablecolourmenu)
        {
            for(std::size_t i=1; i<NUM_BTNS; ++i)
            {
                if(btn[i].is_hit(x,y))
                {
                    selectedcolour = btn[i].c;
                    selectedbtn = i;
                    return;
                }
            }
        }
        currcurve.c = selectedcolour;
        currcurve.thickness = currpenthickness;
    }
    else
    {
        if(enableeraser)
        {
            //int to_remove_idx = -1;
            // int sz = curves.size();
            // for(int i=sz; i>=0; --i)
            // {
            //     if(curves[i].is_near(sx, sy, curves[i].thickness))
            //     {
            //         to_remove_idx = i;
            //         break;
            //     }
            // }
            //             if(to_remove_idx>=0)
            // {
            //     curves.erase(curves.begin()+to_remove_idx);
            // }

            for(auto it=curvesets[currcurveset].begin(); it!=curvesets[currcurveset].end(); ++it)
            {
                if(it->is_near(sx, sy, it->thickness))
                {
                    curvesets[currcurveset].erase(it);
                    break;
                }
            }
        }
        else if(state==SDL_MOUSEBUTTONDOWN)
        {
            currcurve.add_point(sx, sy);
        }
    }
}

void mvcanvas_motion(int x, int y)
{
    if(wasdown)
    {
        float sx = (x-currcenterx)/currscale;
        float sy = (y-currcentery)/currscale;
        if(!enableeraser)
        {
            currcurve.add_point(sx, sy);
        }
        else
        {
            // int to_remove_idx = -1;
            // int sz = curvesets[currcurveset].size();
            // for(int i=sz; i>=0; --i)
            // {
            //     if(curvesets[currcurveset][i].is_near(sx, sy, curvesets[currcurveset][i].thickness))
            //     {
            //         to_remove_idx = i;
            //         break;
            //     }
            // }
            // if(to_remove_idx>=0)
            // {
            //     curvesets[currcurveset].erase(curvesets[currcurveset].begin()+to_remove_idx);
            // }

            for(auto it=curvesets[currcurveset].begin(); it!=curvesets[currcurveset].end(); ++it)
            {
                if(it->is_near(sx, sy, it->thickness))
                {
                    curvesets[currcurveset].erase(it);
                    break;
                }
            }
        }
    }
}

void mvcanvas_resize(int w, int h)
{
	if(h==0) h = 1;
	currscale = h/initheight;
	currcenterx = w/2.0;
	currcentery = h/2.0;
    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, w, h);
    glLoadIdentity();
    glOrtho(-currcenterx, currcenterx, currcentery, -currcentery, -1, 1);
}

void mvcanvas_display()
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-currcenterx, currcenterx, currcentery, -currcentery, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // for(size_t i=0; i<curves.size(); ++i)
    // {
    //     curves[i].draw();
    // }
    for(auto it=curvesets[currcurveset].begin(); it!=curvesets[currcurveset].end(); ++it)
    {
        it->draw();
    }
    currcurve.draw();
    if(enablecolourmenu)
    {
        for(int i=1; i<NUM_BTNS; ++i)
        {
            btn[i].draw(btn[i].x-currcenterx, btn[i].y-currcentery, selectedbtn==i);
        }
    }
    glEnable(GL_DEPTH_TEST);
    if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


