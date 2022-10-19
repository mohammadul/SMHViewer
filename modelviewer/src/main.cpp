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
 * @file main.cpp
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2015-2022 Sk. Mohammadul Haque.
 * @brief This file contains definitions of main structures and functions.
 */

#if !defined(_MSC_VER) && !defined(__MINGW32__)
#include <inttypes.h>
#else
#if defined(__GNUC__)
#define PRId64 "lld"
#else
#define PRId64 "I64d"
#endif
#endif
#include "../include/main.h"

MESH m1 = NULL;
MESH_FACE_NORMAL mfc1 = NULL;
MESH maxis = NULL;
MESH mivn = NULL;
SDL_Window* gWindow = NULL;
SDL_GLContext gContext;
int height = INIT_HEIGHT, width = INIT_WIDTH;
mesh_vector3 pos = {0.0, 0.0, 0.0}, pos1 = {0.0, 0.0, INIT_Z}, trk = {0.0, 0.0, 0.0};
uint8_t smoothshade = 0;
bool needRedraw = true, recalcModelView = true, quit = false;
bool wireframe = false;
bool displayListInited = false;
bool tracking = false;
bool lighting = true;
bool pcloud = false;
bool stereo = false;
bool vnormd = false, fnormd = false;
bool init_fs = false;
bool sabout = false;
bool dside = false;
bool csel = false;
bool objcentric = true;
constexpr bool show_rc = false;
std::vector<Colour> colors;
GLfloat psize = 1.0, vnormscale = 1.0;
GLSTEREOVIEW  sv = NULL;
GLuint indx[2], butt = 0;
GLfloat light_dif[4] = {0.75, 0.75, 0.75, 1.0};
GLfloat clear_col[4] = {0.0, 0.0, 0.0, 1.0};
bool transpar = false;
GLfloat spec = 0.0, spec_color[4] = {0.0, 0.0, 0.0, 1.0};
GLint shininess = INIT_SHININESS;
GLfloat phi = 0.0;
FLOATDATA TRACKBALLSIZE = INIT_TRACKBALLSIZE;
FLOATDATA thrangdeg = 35.0;
quat<FLOATDATA> curr_rot;
quat<FLOATDATA> qpx, qpy, qpz, qnx, qny, qnz;
int fcounter = 0;
GLfloat fov = INIT_FOV, nearplane = INIT_NEARPLANE, farplane = INIT_FARPLANE, posconstant = INIT_Z*tan(INIT_FOV*M_PI/360.0), nconstant = INIT_NEARPLANE*tan(INIT_FOV*M_PI/360.0), fconstant = INIT_FARPLANE*tan(INIT_FOV*M_PI/360.0);
GLfloat baseline = INIT_BASELINE, refdepth = INIT_REFDEPTH;
int stereotype = GLSTEREO_RED_CYAN;
char folder_name[256];
SDL_Thread *thread_about = NULL;

bool colour_generated = false;
bool has_prev_vcolors = false;
bool use_dcolors = false;
bool text_ok = false;
bool show_minfo = false;
GLuint vfbb_tex;
TTF_Font* font = NULL;
SDL_Surface* vfbb = NULL;
int font_size = 12;
char expath[512];
bool show_axis = false;
mesh_vector3 minm = {0.0, 0.0, 0.0}, maxm = {0.0, 0.0, 0.0};
int glVersion[2] = {0, 0};
SDL_Rect srcrect;
uint64_t appver = 20001; /* MAJORMAJORMINORMINORPATCHPATCH (JJNNPP) */

MESH m_rc = NULL;

MESH_COLOR scalar_colors = NULL;
bool configure = false;
char* configure_fname = nullptr;


template<typename T>
void apply_quat(quat<T>& q, MESH_VECTOR3 a, MESH_VECTOR3 b)
{
    FLOATDATA twodt = 2*(a->x*q.x+a->y*q.y+a->z*q.z);
    FLOATDATA ssqr_m_usqr = q.w*q.w-(q.x*q.x+q.y*q.y+q.z*q.z);
    b->x = twodt*q.x+ssqr_m_usqr*a->x+2.0*q.w*(q.y*a->z-a->y*q.z);
    b->y = twodt*q.y+ssqr_m_usqr*a->y+2.0*q.w*(q.z*a->x-a->z*q.x);
    b->z = twodt*q.z+ssqr_m_usqr*a->z+2.0*q.w*(q.x*a->y-a->x*q.y);
}

template<typename T>
void apply_quat_inv(quat<T>& q, MESH_VECTOR3 a, MESH_VECTOR3 b)
{
    FLOATDATA twodt = 2*(a->x*q.x+a->y*q.y+a->z*q.z);
    FLOATDATA ssqr_m_usqr = q.w*q.w-(q.x*q.x+q.y*q.y+q.z*q.z);
    b->x = twodt*q.x+ssqr_m_usqr*a->x-2.0*q.w*(q.y*a->z-a->y*q.z);
    b->y = twodt*q.y+ssqr_m_usqr*a->y-2.0*q.w*(q.z*a->x-a->z*q.x);
    b->z = twodt*q.z+ssqr_m_usqr*a->z-2.0*q.w*(q.x*a->y-a->x*q.y);
}

char *extract_filename(char *str)
{
    DEBUG_STDOUT("EXTRACT_FILENAME\n")
#if defined(__WIN32) || defined(__WIN32__) ||defined(WIN32) || defined(WINNT)
    int ch = '\\';
#else
    int ch = '/';
#endif
    char *pdest = NULL;
    pdest = strrchr(str, ch);
    if(pdest==NULL) pdest = str;
    else ++pdest;
    return pdest;
}

MESH_FACE_NORMAL mesh_calc_face_normals2(MESH m, FLOATDATA thrangledeg)
{
    DEBUG_STDOUT("MESH_CALC_FACE_NORMALS2\n")
    MESH_FACE_NORMAL mfc = NULL;
    INTDATA i;
    FLOATDATA thresh = cos(thrangledeg*MESH_PI/180.0);
    if(m==NULL||m->is_faces==0) return NULL;
    if(m->is_fnormals==0) mesh_calc_face_normals(m);
    if(m->is_vfaces==0) mesh_calc_vertex_adjacency(m);
    if((mfc = (MESH_FACE_NORMAL)malloc(sizeof(mesh_face_normal)*m->num_faces))==NULL) mesh_error(MESH_ERR_MALLOC);
    #pragma omp parallel for
    for(i=0; i<m->num_faces; ++i)
    {
        MESH_FACE cf = m->faces+i;
        INTDATA j;
        MESH_NORMAL cfn = m->fnormals+i;
        if((mfc[i].cn = (MESH_NORMAL)malloc(sizeof(mesh_normal)*cf->num_vertices))==NULL) mesh_error(MESH_ERR_MALLOC);
        memset(mfc[i].cn, 0, sizeof(mesh_normal)*cf->num_vertices);
        for(j=0; j<cf->num_vertices; ++j)
        {
            INTDATA k;
            INTDATA cvj = cf->vertices[j];
            MESH_VFACE cfvf = m->vfaces+cvj;
            MESH_NORMAL mfcfv = mfc[i].cn+j;
            FLOATDATA t;
            for(k=0; k<cfvf->num_faces; ++k)
            {
                INTDATA cfvfk = cfvf->faces[k];
                MESH_NORMAL cfvfn = m->fnormals+cfvfk;
                FLOATDATA dp = cfn->x*cfvfn->x+cfn->y*cfvfn->y+cfn->z*cfvfn->z;
                if(dp>=thresh)
                {
                    mfcfv->x += cfvfn->x;
                    mfcfv->y += cfvfn->y;
                    mfcfv->z += cfvfn->z;
                }
            }
            t = sqrt(mfcfv->x*mfcfv->x+mfcfv->y*mfcfv->y+mfcfv->z*mfcfv->z);
            if(t>0.0)
            {
                mfcfv->x /= t;
                mfcfv->y /= t;
                mfcfv->z /= t;
            }
        }
    }
    return mfc;
}

void mesh_free_face_normals2()
{
    DEBUG_STDOUT("MESH_FREE_FACE_NORMALS2\n")
    INTDATA i;
    if(mfc1!=NULL)
    {
        for(i=0; i<m1->num_faces; ++i)
        {
            free(mfc1[i].cn);
        }
        free(mfc1);
        mfc1 = NULL;
    }
}

void mesh_draw_mesh_smooth2(MESH m, MESH_FACE_NORMAL mfc)
{
    DEBUG_STDOUT("MESH_DRAW_MESH_SMOOTH2\n")
    INTDATA i, j;
    GLfloat currcolor[4];
    if(mfc==NULL) return;
    if(m->is_trimesh)
    {
        if(m->is_fcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                MESH_NORMAL cfn = mfc[i].cn;
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glColor3fv(currcolor);
                glNormal3f(cfn[0].x, cfn[0].y, cfn[0].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(cfn[1].x, cfn[1].y, cfn[1].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glNormal3f(cfn[2].x, cfn[2].y, cfn[2].z);
                glVertex3f(m->vertices[m->faces[i].vertices[2]].x, m->vertices[m->faces[i].vertices[2]].y, m->vertices[m->faces[i].vertices[2]].z);
            }
            glEnd();
            glDisable(GL_COLOR_MATERIAL);
        }
        else if(m->is_vcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                MESH_NORMAL cfn = mfc[i].cn;
                currcolor[0] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].r+m->vcolors[m->faces[i].vertices[1]].r+m->vcolors[m->faces[i].vertices[2]].r);
                currcolor[1] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].g+m->vcolors[m->faces[i].vertices[1]].g+m->vcolors[m->faces[i].vertices[2]].g);
                currcolor[2] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].b+m->vcolors[m->faces[i].vertices[1]].b+m->vcolors[m->faces[i].vertices[2]].b);
                currcolor[3] = 0.33333*(m->vcolors[m->faces[i].vertices[0]].a+m->vcolors[m->faces[i].vertices[1]].a+m->vcolors[m->faces[i].vertices[2]].a);
                glColor3fv(currcolor);
                glNormal3f(cfn[0].x, cfn[0].y, cfn[0].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(cfn[1].x, cfn[1].y, cfn[1].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glNormal3f(cfn[2].x, cfn[2].y, cfn[2].z);
                glVertex3f(m->vertices[m->faces[i].vertices[2]].x, m->vertices[m->faces[i].vertices[2]].y, m->vertices[m->faces[i].vertices[2]].z);
            }
            glEnd();
            glDisable(GL_COLOR_MATERIAL);
        }
        else
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            currcolor[0] = 1.0;
            currcolor[1] = 1.0;
            currcolor[2] = 1.0;
            currcolor[3] = 1.0;
            glColor3fv(currcolor);
            glBegin(GL_TRIANGLES);
            for(i=0; i<m->num_faces; ++i)
            {
                MESH_NORMAL cfn = mfc[i].cn;
                glNormal3f(cfn[0].x, cfn[0].y, cfn[0].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(cfn[1].x, cfn[1].y, cfn[1].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                glNormal3f(cfn[2].x, cfn[2].y, cfn[2].z);
                glVertex3f(m->vertices[m->faces[i].vertices[2]].x, m->vertices[m->faces[i].vertices[2]].y, m->vertices[m->faces[i].vertices[2]].z);
            }
            glEnd();
            glDisable(GL_COLOR_MATERIAL);
        }
    }
    else if(m->num_faces>0)
    {
        if(m->is_fcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            for(i=0; i<m->num_faces; ++i)
            {
                MESH_NORMAL cfn = mfc[i].cn;
                currcolor[0] = m->fcolors[i].r;
                currcolor[1] = m->fcolors[i].g;
                currcolor[2] = m->fcolors[i].b;
                currcolor[3] = m->fcolors[i].a;
                glColor3fv(currcolor);
                glBegin(GL_TRIANGLE_FAN);
                glNormal3f(cfn[0].x, cfn[0].y, cfn[0].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(cfn[1].x, cfn[1].y, cfn[1].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                for(j=2; j<m->faces[i].num_vertices; ++j)
                {
                    glNormal3f(cfn[j].x, cfn[j].y, cfn[j].z);
                    glVertex3f(m->vertices[m->faces[i].vertices[j]].x, m->vertices[m->faces[i].vertices[j]].y, m->vertices[m->faces[i].vertices[j]].z);
                }
                glEnd();
            }
            glDisable(GL_COLOR_MATERIAL);
        }
        if(m->is_vcolors)
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            for(i=0; i<m->num_faces; ++i)
            {
                MESH_NORMAL cfn = mfc[i].cn;
                glBegin(GL_TRIANGLE_FAN);
                for(j=0; j<m->faces[i].num_vertices; ++j)
                {
                    INTDATA idx = m->faces[i].vertices[j];
                    currcolor[0] = m->vcolors[idx].r;
                    currcolor[1] = m->vcolors[idx].g;
                    currcolor[2] = m->vcolors[idx].b;
                    currcolor[3] = m->vcolors[idx].a;
                    glColor3fv(currcolor);
                    glNormal3f(cfn[j].x, cfn[j].y, cfn[j].z);
                    glVertex3f(m->vertices[idx].x, m->vertices[idx].y, m->vertices[idx].z);
                }
                glEnd();

            }
            glDisable(GL_COLOR_MATERIAL);
        }
        else
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            currcolor[0] = 1.0;
            currcolor[1] = 1.0;
            currcolor[2] = 1.0;
            currcolor[3] = 1.0;
            glColor3fv(currcolor);
            for(i=0; i<m->num_faces; ++i)
            {
                MESH_NORMAL cfn = mfc[i].cn;
                glBegin(GL_TRIANGLE_FAN);
                glNormal3f(cfn[0].x, cfn[0].y, cfn[0].z);
                glVertex3f(m->vertices[m->faces[i].vertices[0]].x, m->vertices[m->faces[i].vertices[0]].y, m->vertices[m->faces[i].vertices[0]].z);
                glNormal3f(cfn[1].x, cfn[1].y, cfn[1].z);
                glVertex3f(m->vertices[m->faces[i].vertices[1]].x, m->vertices[m->faces[i].vertices[1]].y, m->vertices[m->faces[i].vertices[1]].z);
                for(j=2; j<m->faces[i].num_vertices; ++j)
                {
                    glNormal3f(cfn[j].x, cfn[j].y, cfn[j].z);
                    glVertex3f(m->vertices[m->faces[i].vertices[j]].x, m->vertices[m->faces[i].vertices[j]].y, m->vertices[m->faces[i].vertices[j]].z);
                }
                glEnd();
            }
            glDisable(GL_COLOR_MATERIAL);
        }
    }
}

void init_current_file()
{
    DEBUG_STDOUT("INIT_CURRENT_FILE\n")
    FILE* search_file = NULL;
    char search_filename[256];
#if defined(__WIN32) || defined(__WIN32__) ||defined(WIN32) || defined(WINNT)
    sprintf(folder_name, "%s\\", ".");
#else
    sprintf(folder_name, "%s/", ".");
#endif
    fcounter = -1;
    do
    {
        if(search_file)
        {
            fclose(search_file);
        }
        ++fcounter;
#if defined(__WIN32) || defined(__WIN32__) ||defined(WIN32) || defined(WINNT)
        sprintf(search_filename, "%s" STR(FNAME_PREFIX) "%d.png", folder_name, fcounter);
#else
        sprintf(search_filename, "%s" STR(FNAME_PREFIX) "%d.png", folder_name, fcounter);
#endif
        search_file = fopen(search_filename, "r");
    }
    while(search_file);
}

void set_icon(SDL_Window* w)
{
    DEBUG_STDOUT("SET_ICON\n")
#if defined(__WIN32) || defined(__WIN32__) ||defined(WIN32) || defined(WINNT)
    HICON icon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101));
    if(icon)
    {
        ICONINFO ici;
        if(GetIconInfo(icon, &ici))
        {
            HDC dc = CreateCompatibleDC(NULL);
            if(dc)
            {
                SDL_Surface* ic = SDL_CreateRGBSurface(0, 32, 32, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                if(ic)
                {
                    BITMAPINFO bmi;
                    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmi.bmiHeader.biWidth = 32;
                    bmi.bmiHeader.biHeight = -32;
                    bmi.bmiHeader.biPlanes = 1;
                    bmi.bmiHeader.biBitCount = 32;
                    bmi.bmiHeader.biCompression = BI_RGB;
                    bmi.bmiHeader.biSizeImage = 0;
                    SelectObject(dc, ici.hbmColor);
                    GetDIBits(dc, ici.hbmColor, 0, 32, ic->pixels, &bmi, DIB_RGB_COLORS);
                    SDL_SetWindowIcon(w, ic);
                    SDL_FreeSurface(ic);
                }
                DeleteDC(dc);
            }
            DeleteObject(ici.hbmColor);
            DeleteObject(ici.hbmMask);
        }
        DestroyIcon(icon);
    }
#endif
}

void init_ttf(const char* fontname)
{
    DEBUG_STDOUT("INIT_TTF\n")
    text_ok = false;
    if(TTF_Init()==-1) return;
    if((font = TTF_OpenFont(fontname, font_size))==NULL)
    {
        TTF_Quit();
        return;
    }
    text_ok = true;
}

void generate_text(MESH m, MESH_VECTOR3 mnm, MESH_VECTOR3 mxm)
{
    DEBUG_STDOUT("GENERATE_TEXT\n")
    char text[512];
#if MESH_INTDATA_TYPE==0
    sprintf(text, " #Vertices: %d\n #Faces: %d\n Min: (%g,%g,%g)\n Max: (%g,%g,%g)\n OpenGL %d.%d ", m->num_vertices, m->num_faces, mnm->x, mnm->y, mnm->z, mxm->x, mxm->y, mxm->z, glVersion[0], glVersion[1]);
#else
    sprintf(text, " #Vertices: %" PRId64 "\n #Faces: %" PRId64 "\n Min: (%g,%g,%g)\n Max: (%g,%g,%g) \n OpenGL %d.%d ", m->num_vertices, m->num_faces, mnm->x, mnm->y, mnm->z, mxm->x, mxm->y, mxm->z, glVersion[0], glVersion[1]);
#endif
    int mxl = 0, lstl = 0, lgth = strlen(text);
    for(int i=0; i<lgth; ++i)
    {
        if(text[i]!='\n') ++lstl;
        else
        {
            if(lstl>mxl) mxl = lstl;
            lstl = 0;
        }
    }
    SDL_Color tcol;
    tcol.r = 255*(1.0-clear_col[0]);
    tcol.g = 255*(1.0-clear_col[1]);
    tcol.b = 255*(1.0-clear_col[2]);
    tcol.a = 255;
    SDL_Surface* vfbb_;
    if((vfbb_ = TTF_RenderText_Blended_Wrapped(font, text, tcol, width))==NULL)
    {
        text_ok = false;
        TTF_Quit();
        return;
    };
    srcrect.h = vfbb_->h;
    srcrect.w = static_cast<int>(0.52*font_size*(mxl));
    srcrect.x = 0;
    srcrect.y = 0;
    if(glVersion[0]<2)
    {
        int h_ = pow(2, ceil(log(srcrect.h)/log(2)));
        int w_ = pow(2, ceil(log(srcrect.w)/log(2)));
        srcrect.x = w_-srcrect.w;
        srcrect.y = h_-srcrect.h;
        srcrect.h = h_;
        srcrect.w = w_;
    }
    if(vfbb!=NULL) SDL_FreeSurface(vfbb);
    vfbb = SDL_CreateRGBSurface(vfbb_->flags, srcrect.w, srcrect.h, 32, vfbb_->format->Rmask, vfbb_->format->Gmask, vfbb_->format->Bmask, vfbb_->format->Amask);
    SDL_FillRect(vfbb, NULL, SDL_MapRGBA(vfbb_->format, 255*clear_col[0], 255*clear_col[1], 255*clear_col[2], 127));
    SDL_BlitSurface(vfbb_, NULL, vfbb, NULL);
    SDL_FreeSurface(vfbb_);

    if(glIsTexture(vfbb_tex)==GL_TRUE) glDeleteTextures(1, &vfbb_tex);
    glGenTextures(1, &vfbb_tex);
    glBindTexture(GL_TEXTURE_2D, vfbb_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vfbb->w, vfbb->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, vfbb->pixels);
}

void render_text(int x, int y)
{
    DEBUG_STDOUT("RENDER_TEXT\n")
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, vfbb_tex);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0,1);
        glVertex2f(x, y);
        glTexCoord2f(1,1);
        glVertex2f(x+vfbb->w, y);
        glTexCoord2f(1,0);
        glVertex2f(x+vfbb->w, y+vfbb->h);
        glTexCoord2f(0,0);
        glVertex2f(x, y+vfbb->h);
    }
    glEnd();

    if(lighting) glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

MESH generate_axes(FLOATDATA sc)
{
    DEBUG_STDOUT("GENERATE_AXES\n")
    MESH m_ = NULL, m_ax = NULL;
    MESH_COLOR mfc = NULL;
    mesh_vector3 sz, ps;

    sz.x = 1.0;
    sz.y = sc;
    sz.z  = sc;
    ps.x = 0.5;
    ps.y = 0.0;
    ps.z = 0.0;
    m_ = mesh_create_mesh_new_cuboid(&sz, &ps);
    m_->is_fcolors = 1;
    if((m_->fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*m_->num_faces))==NULL) mesh_error(MESH_ERR_MALLOC);
    mfc = m_->fcolors;
    for(int i=0; i<m_->num_faces; ++i)
    {
        mfc[i].r = 0.95;
        mfc[i].g = 0.05;
        mfc[i].b = 0.05;
        mfc[i].a = 1.0;
    }
    m_ax = mesh_combine_mesh(m_ax, m_);
    mesh_free_mesh(m_);
    sz.x  = sc;
    sz.y = 1.0;
    sz.z  = sc;
    ps.x = 0.0;
    ps.y = 0.5;
    ps.z = 0.0;
    m_ = mesh_create_mesh_new_cuboid(&sz, &ps);
    m_->is_fcolors = 1;
    if((m_->fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*m_->num_faces))==NULL) mesh_error(MESH_ERR_MALLOC);
    mfc = m_->fcolors;
    for(int i=0; i<m_->num_faces; ++i)
    {
        mfc[i].r = 0.05;
        mfc[i].g = 0.95;
        mfc[i].b = 0.05;
        mfc[i].a = 1.0;
    }
    m_ax = mesh_combine_mesh(m_ax, m_);
    mesh_free_mesh(m_);
    sz.x  = sc;
    sz.y  = sc;
    sz.z = 1.0;
    ps.x = 0.0;
    ps.y = 0.0;
    ps.z = 0.5;
    m_ = mesh_create_mesh_new_cuboid(&sz, &ps);
    m_->is_fcolors = 1;
    if((m_->fcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*m_->num_faces))==NULL) mesh_error(MESH_ERR_MALLOC);
    mfc = m_->fcolors;
    for(int i=0; i<m_->num_faces; ++i)
    {
        mfc[i].r = 0.05;
        mfc[i].g = 0.05;
        mfc[i].b = 0.95;
        mfc[i].a = 1.0;
    }
    m_ax = mesh_combine_mesh(m_ax, m_);
    mesh_free_mesh(m_);
    return m_ax;
}

void get_exec_path(char* buffer)
{
    DEBUG_STDOUT("GET_EXEC_PATH\n")
    char* path_end;
    int ptsz;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    path_end = strrchr(buffer, '\\');
#else
    path_end = strrchr(buffer, '/');
#endif
    if(path_end==NULL)
    {
        expath[0] = '.';
        ptsz = 1;
    }
    else
    {
        ptsz = (path_end-buffer);
        memcpy(expath, buffer, ptsz);
    }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    expath[ptsz] = '\\';
#else
    expath[ptsz] = '/';
#endif
    expath[ptsz+1] = '\0';
}

void init()
{
    DEBUG_STDOUT("INIT\n")
    if(SDL_Init(SDL_INIT_VIDEO)<0) fatalerror("SDL could not initialize.");
    else
    {
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        glEnable(GL_MULTISAMPLE);
        if(init_fs) gWindow = SDL_CreateWindow("SMHViewer ModelViewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_FULLSCREEN_DESKTOP);
        else gWindow = SDL_CreateWindow("SMHViewer ModelViewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
        if(gWindow==NULL) fatalerror("Window could not be created.");
        else
        {
            set_icon(gWindow);
            gContext = SDL_GL_CreateContext(gWindow);
            if(gContext==NULL) fatalerror("OpenGL context could not be created.");
            else
            {
                if(SDL_GL_SetSwapInterval(1)<0) warning("Unable to set VSync.");
                initGL();
                mvcanvas_init();
            }
        }
    }
    char fntpth[512];
    sprintf(fntpth, "%sLato-Regular.ttf",expath);
    init_ttf(fntpth);
    init_current_file();
}

void initGL()
{
    DEBUG_STDOUT("INITGL\n")
    GLfloat light_pos[4] = {0.0, 0.0, 10.0, 0.0}, light_amb[4] = {0.05, 0.05, 0.05, 1.0};
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
    sv = glstereo_new(baseline, refdepth, fov, nearplane, farplane, width/static_cast<FLOATDATA>(height), GLSTEREO_RED_CYAN);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, width/static_cast<FLOATDATA>(height), nearplane, farplane);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(pos.x, pos.y, pos.z);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_amb);
    glShadeModel(GL_FLAT);
    if(lighting) glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glEnable(GL_LIGHT0);
    qpx.fromangleaxis(1.0, 0.0, 0.0, M_PI/90.0);
    qpy.fromangleaxis(0.0, 1.0, 0.0, M_PI/90.0);
    qpz.fromangleaxis(0.0, 0.0, 1.0, M_PI/90.0);
    qnx.fromangleaxis(1.0, 0.0, 0.0, -M_PI/90.0);
    qny.fromangleaxis(0.0, 1.0, 0.0, -M_PI/90.0);
    qnz.fromangleaxis(0.0, 0.0, 1.0, -M_PI/90.0);
}

void check_colour_gen()
{
    DEBUG_STDOUT("CHECK_COLOUR_GEN\n")
    if(!colour_generated && m1->is_vscalars)
    {
        INTDATA count_colors = 0;
        for(INTDATA i=0; i<m1->num_vertices; ++i)
        {
            if(count_colors<(m1->vscalars[i]-MESH_EPS12))
                count_colors = std::round(m1->vscalars[i]);
        }
        if(count_colors<1) count_colors = 1;
        distinctcolours(count_colors+1, colors);
        if(m1->is_vcolors) has_prev_vcolors = true;
        else m1->vcolors = NULL; // assure that swapping will be safe
        if((scalar_colors = (MESH_COLOR) malloc(sizeof(mesh_color)*m1->num_vertices))==NULL) mesh_error(MESH_ERR_MALLOC);
        #pragma omp parallel for
        for(INTDATA i=0; i<m1->num_vertices; ++i)
        {
            INTDATA cidx = std::round(m1->vscalars[i]);
            if(cidx<0) cidx = 0;
            scalar_colors[i].r = colors[cidx].rgb[0];
            scalar_colors[i].g = colors[cidx].rgb[1];
            scalar_colors[i].b = colors[cidx].rgb[2];
            scalar_colors[i].a = 1.0;
        }
        colour_generated = true;
    }
}

void swap_vcolors_with_vscalars()
{
    DEBUG_STDOUT("SWAP_VCOLORS_WITH_VSCALARS\n")
    if(m1->is_vscalars)
    {
        check_colour_gen();
        if(use_dcolors) m1->is_vcolors = 1;
        else if(!has_prev_vcolors) m1->is_vcolors = 0;
        std::swap(m1->vcolors, scalar_colors);
    }
}

void load_files(int num, char* fnames[])
{
    DEBUG_STDOUT("LOAD_FILES\n")
    mesh_vector3 minv = {FLT_MAX,FLT_MAX,FLT_MAX}, maxv = {-FLT_MAX,-FLT_MAX,-FLT_MAX}, center;
    FLOATDATA scale;
    MESH m2 = NULL;
    m1 = NULL;
    bool color_scale = false;
    char wtitle[256], *ffname = NULL;
    for(int i=0; i<num; ++i)
    {
        m2 = mesh_load_file(fnames[i]);
        m1 = mesh_combine_mesh(m1, m2);
        mesh_free_mesh(m2);
    }

    ffname = extract_filename(fnames[0]);
#if defined(__WIN32) || defined(__WIN32__) ||defined(WIN32) || defined(WINNT)
    sprintf(wtitle, "SMHViewer ModelViewer - %s", ffname);
#else
    sprintf(wtitle, "SMHViewer ModelViewer - %s", ffname);
#endif
    SDL_SetWindowTitle(gWindow, wtitle);
    for(int i=0; i<m1->num_vertices; ++i)
    {
        MESH_VERTEX v = m1->vertices+i;
        if(v->x>maxv.x) maxv.x = v->x;
        if(v->y>maxv.y) maxv.y = v->y;
        if(v->z>maxv.z) maxv.z = v->z;
        if(v->x<minv.x) minv.x = v->x;
        if(v->y<minv.y) minv.y = v->y;
        if(v->z<minv.z) minv.z = v->z;
        if(m1->is_vcolors && (m1->vcolors[i].r>1 ||m1->vcolors[i].g>1||m1->vcolors[i].b>1)) color_scale = true;
    }
    if(m1->is_vcolors && color_scale)
    {
        #pragma omp parallel for
        for(int i=0; i<m1->num_vertices; ++i)
        {
            m1->vcolors[i].r /= 256.0;
            m1->vcolors[i].g /= 256.0;
            m1->vcolors[i].b /= 256.0;
        }
    }

    center.x = -0.5*(minv.x+maxv.x);
    center.y = -0.5*(minv.y+maxv.y);
    center.z = -0.5*(minv.z+maxv.z);
    minm.x = minv.x;
    minm.y = minv.y;
    minm.z = minv.z;
    maxm.x = maxv.x;
    maxm.y = maxv.y;
    maxm.z = maxv.z;
    if(text_ok) generate_text(m1, &minm, &maxm);
    maxv.x = fabs(maxv.x-minv.x);
    maxv.y = fabs(maxv.y-minv.y);
    maxv.z = fabs(maxv.z-minv.z);
    maxv.x = (maxv.x>maxv.y)?maxv.x:maxv.y;
    maxv.x = (maxv.x>maxv.z)?maxv.x:maxv.z;
    scale = 1.2/maxv.x;
    mesh_translate_vector(m1, &center);
    mesh_scale(m1, scale, scale, scale);
    maxis = generate_axes(0.007);
    center.x *= scale;
    center.y *= scale;
    center.z *= scale;
    mesh_translate_vector(maxis, &center);
    mesh_vector3 szz = {0.01, 0.01, 0.01}, poss = {0.0, 0.0, 0.0};
    m_rc = mesh_create_mesh_new_cone(&szz, &poss);

    if(!m1->is_faces)
    {
        pcloud = true;
        lighting = false;
        glDisable(GL_LIGHTING);
    }
    else
    {
        mesh_calc_face_normals(m1);
    }
}

void load_infra(const char* fname)
{
    DEBUG_STDOUT("LOAD_INFRA\n")
    mvcfg_load(fname);
    int numcfgs = cfgs.cwrds.size();
    unsigned char *pixels;

    for(int i=0; i<numcfgs; ++i)
    {
        mvcfgword& ccwrd = cfgs.cwrds[i];
        /* actors */
        switch(ccwrd.codenum)
        {
        case CMD_PREV_MODEL:
            break;
        case CMD_NEXT_MODEL:
            break;
        case CMD_EXIT:
            quit = true;
            return;
        case CMD_HEIGHT:
            height = ccwrd.codevals[0];;
            SDL_SetWindowSize(gWindow, width, height);
            SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            break;

        case CMD_WIDTH:
            width = ccwrd.codevals[0];
            SDL_SetWindowSize(gWindow, width, height);
            SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            break;

        case CMD_SHININESS:
            if(ccwrd.codevals[0]>=5.0 && (int)ccwrd.codevals[0]<=100.0)
            {
                shininess = ccwrd.codevals[0];
                needRedraw = true;
                if(displayListInited)
                {
                    glDeleteLists(indx[0], 1);
                    glDeleteLists(indx[1], 1);
                }
                displayListInited = false;
            }
            break;

        case CMD_SPEC:
            if(ccwrd.codevals[0]>=0.0 && ccwrd.codevals[0]<=1.0)
            {
                spec = ccwrd.codevals[0];
                spec_color[0] = spec;
                spec_color[1] = spec;
                spec_color[2] = spec;
                needRedraw = true;
                if(displayListInited)
                {
                    glDeleteLists(indx[0], 1);
                    glDeleteLists(indx[1], 1);
                }
                displayListInited = false;
            }
            break;

        case CMD_FOV:
            if(ccwrd.codevals[0]>=2.0 && ccwrd.codevals[0]<=90.0)
            {
                fov = ccwrd.codevals[0];
                FLOATDATA sc = 1.0/tan(fov*M_PI/360.0);
                pos.z = posconstant*sc;
                nearplane = nconstant*sc;
                farplane = fconstant*sc;
                recalcModelView = true;
            }
            break;

         case CMD_PSIZE:
            if(ccwrd.codevals[0]>=1.0 && ccwrd.codevals[0]<=10.0)
            {
                psize = ccwrd.codevals[0];
                glPointSize(psize);
                if(displayListInited)
                {
                    glDeleteLists(indx[0], 1);
                    glDeleteLists(indx[1], 1);
                }
                displayListInited = false;
                needRedraw = true;
            }
            break;

         case CMD_PCLOUD:
            pcloud = (int)ccwrd.codevals[0];
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
            needRedraw = true;
            break;

        case CMD_WIREFRAME:
            wireframe = (int)ccwrd.codevals[0];
            if(pcloud)
            {
                pcloud = false;
                if(displayListInited)
                {
                    glDeleteLists(indx[0], 1);
                    glDeleteLists(indx[1], 1);
                }
                displayListInited = false;
            }
            if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            needRedraw = true;
            break;

        case CMD_AXIS:
            show_axis = (int)ccwrd.codevals[0];
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
            needRedraw = true;
            break;

        case CMD_LIGHTING:
            lighting = (int)ccwrd.codevals[0];
            if(lighting) glEnable(GL_LIGHTING);
            else glDisable(GL_LIGHTING);
            needRedraw = true;
            break;

        case CMD_SEMANTICS:
            use_dcolors = (int)ccwrd.codevals[0];
            swap_vcolors_with_vscalars();
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
            recalcModelView = true;
            needRedraw = true;
            break;

        case CMD_TRANSBG:
            transpar = (int)ccwrd.codevals[0];
            if(transpar) clear_col[3] = 0.0;
            else clear_col[3] = 1.0;
            glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
            needRedraw = true;
            break;

        case CMD_INFO:
            show_minfo = (int)ccwrd.codevals[0];
            if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
            needRedraw = true;
            break;

        case CMD_RENDER_TYPE:
            if((int)ccwrd.codevals[0]>=0 && (int)ccwrd.codevals[0]<3)
            {
                smoothshade = (int)ccwrd.codevals[0];
                pcloud = false;
                if(displayListInited)
                {
                    glDeleteLists(indx[0], 1);
                    glDeleteLists(indx[1], 1);
                }
                if(smoothshade>0) glShadeModel(GL_SMOOTH);
                else glShadeModel(GL_FLAT);
                if(smoothshade==2 && mfc1 ==NULL)
                {
                    mfc1 = mesh_calc_face_normals2(m1, thrangdeg);
                }
                displayListInited = false;
                needRedraw = true;
            }
            break;

        case CMD_SAVE:
            render(); /* force do render before capture */
            if((pixels = (unsigned char*) malloc(width*height*4))==NULL) warning("Cannot allocate memory for screenshot.");
            else
            {
                glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                // printf("filename: %s len: %d\n", ccwrd.codetxts[0].c_str(), ccwrd.codetxts[0].length());
                tiny_png_save(ccwrd.codetxts[0].c_str(), width, height, TINY_PNG_RGBA, pixels, TINY_PNG_ORDER_UP);
                free(pixels);
            }
            break;

       case CMD_BG_COLOUR:
           if((ccwrd.codevals[0]>=0.0 && ccwrd.codevals[0]<=1.0)
              &&(ccwrd.codevals[1]>=0.0 && ccwrd.codevals[1]<=1.0)
              &&(ccwrd.codevals[2]>=0.0 && ccwrd.codevals[2]<=1.0))
            {
                clear_col[0] = ccwrd.codevals[0];
                clear_col[1] = ccwrd.codevals[1];
                clear_col[2] = ccwrd.codevals[2];
                glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
                if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
                needRedraw = true;
            }
            break;

       case CMD_LT_COLOUR:
           if((ccwrd.codevals[0]>=0.0 && ccwrd.codevals[0]<=1.0)
              &&(ccwrd.codevals[1]>=0.0 && ccwrd.codevals[1]<=1.0)
              &&(ccwrd.codevals[2]>=0.0 && ccwrd.codevals[2]<=1.0))
            {
                light_dif[0] = ccwrd.codevals[0];
                light_dif[1] = ccwrd.codevals[1];
                light_dif[2] = ccwrd.codevals[2];
                glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
                if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
                needRedraw = true;
            }
            break;

       case CMD_POS:
            pos.x = ccwrd.codevals[0];
            pos.y = ccwrd.codevals[1];
            pos.z = ccwrd.codevals[2];
            posconstant = pos.z*tan(fov*M_PI/360.0);
            nconstant = nearplane*tan(fov*M_PI/360.0);
            fconstant = farplane*tan(fov*M_PI/360.0);
            recalcModelView = true;
            needRedraw = true;
            break;

        case CMD_ROT:
            {
                float ax = ccwrd.codevals[0], ay = ccwrd.codevals[1], az = ccwrd.codevals[2];
                double ang = std::sqrt(ax*ax+ay*ay+az*az);
                double d;
                if(ang>0)
                {
                    ax = ax/ang;
                    ay = ay/ang;
                    az = az/ang;
                }
                ang /= 2.0;
                d = std::sin(ang);
                curr_rot.x = ax*d;
                curr_rot.y = ay*d;
                curr_rot.z = az*d;
                curr_rot.w = std::cos(ang);
	            recalcModelView = true;
                needRedraw = true;
            }
            break;
        }

        if(recalcModelView) update();
        if(needRedraw)
        {
            render();
            SDL_GL_SwapWindow(gWindow);
            needRedraw = false;
        }
    }
}

int show_about(void* ptr)
{
    DEBUG_STDOUT("SHOW_ABOUT\n")
    sabout = true;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "About SMHViewer ModelViewer", "SMHViewer 2.0.1 ModelViewer                            \nAn OFF/PLY/ASC/XYZ/BINv1/BundlerOUT/NVM Model Viewer\nCopyright (C) 2022 Sk. Mohammadul Haque\nThis software uses MeshLib, GLStereo, SDL 2.0, SDL_ttf, FreeType, LibPNG and Zlib libraries.\n\nSee documentation for help.\nMIT License.\n\n", gWindow);
    sabout = false;
    return 0;
}

void handle_keys(SDL_Keycode key)
{
    DEBUG_STDOUT("HANDLE_KEYS\n")
    if(enablecanvas)
    {
        mvcanvas_keyb(key);
        needRedraw = true;
        return;
    }
    switch(key)
    {

    case SDLK_HOME:
        enablecanvas = !enablecanvas;
        tracking = false;
        butt = 0;
        needRedraw = true;
        break;

    case SDLK_DELETE:
        if(objcentric)
        {
            mesh_vector3 px = pos;
            apply_quat_inv(curr_rot, &px, &pos);
            objcentric = false;
        }
        else
        {
            mesh_vector3 px = pos;
            apply_quat(curr_rot, &px, &pos);
            objcentric = true;;
        }
        recalcModelView = true;
        needRedraw = true;
        break;

    case SDLK_END:
        use_dcolors=!use_dcolors;
        swap_vcolors_with_vscalars();
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        displayListInited = false;
        recalcModelView = true;
        needRedraw = true;
        break;

//    case SDLK_INSERT:
//        show_rc =!show_rc;
//        recalcModelView = true;
//        needRedraw = true;
//        break;

    case SDLK_QUOTE:
        dside = !dside;
        if(dside) glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        else glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        if(pcloud)
        {
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
        }
        needRedraw = true;
        break;
    case SDLK_l:
        vnormd = !vnormd;
        if(vnormd&&(m1->is_vnormals==0)&&m1->is_faces) mesh_calc_vertex_normals(m1);
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        displayListInited = false;
        needRedraw = true;
        break;
    case SDLK_f:
        fnormd = !fnormd;
        if(fnormd&&(m1->is_fnormals==0)&&m1->is_faces) mesh_calc_face_normals(m1);
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        displayListInited = false;
        needRedraw = true;
        break;
    case SDLK_o:
        if(vnormd||fnormd)
        {
            vnormscale *= 0.666666666666;
            if(vnormscale<0.000005215095050846564) vnormscale = 0.000005215095050846564;
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
            needRedraw = true;
        }
        break;
    case SDLK_p:
        if(vnormd||fnormd)
        {
            vnormscale *= 1.5;
            if(vnormscale>191751.0592328841) vnormscale = 191751.0592328841;
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
            needRedraw = true;
        }
        break;
    case SDLK_KP_PLUS:
        ++psize;
        if(psize>10.0) psize = 10.0;
        glPointSize(psize);
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        displayListInited = false;
        needRedraw = true;
        break;
    case SDLK_KP_MINUS:
        --psize;
        if(psize<1.0) psize = 1.0;
        glPointSize(psize);
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        displayListInited = false;
        needRedraw = true;
        break;
    case SDLK_q:
        quit = true;
        break;
    case SDLK_BACKQUOTE:
        stereo = !stereo;
        recalcModelView = true;
        break;
    case SDLK_SLASH:
        if(stereo)
        {
            if(stereotype==GLSTEREO_RED_CYAN) stereotype = GLSTEREO_GREEN_MAGENTA;
            else stereotype = GLSTEREO_RED_CYAN;
            recalcModelView = true;
        }
        break;
    case SDLK_a:
        curr_rot = qny*curr_rot;
        recalcModelView = true;
        break;
    case SDLK_d:
        curr_rot = qpy*curr_rot;
        recalcModelView = true;
        break;
    case SDLK_w:
        curr_rot = qnx*curr_rot;
        recalcModelView = true;
        break;
    case SDLK_s:
        curr_rot = qpx*curr_rot;
        recalcModelView = true;
        break;
    case SDLK_x:
        curr_rot = qnz*curr_rot;
        recalcModelView = true;
        break;
    case SDLK_z:
        curr_rot = qpz*curr_rot;
        recalcModelView = true;
        break;
    case SDLK_UP:
        pos.z -= 0.05;
        posconstant = pos.z*tan(fov*M_PI/360.0);
        nconstant = nearplane*tan(fov*M_PI/360.0);
        fconstant = farplane*tan(fov*M_PI/360.0);
        recalcModelView = true;
        break;
    case SDLK_DOWN:
        pos.z += 0.05;
        posconstant = pos.z*tan(fov*M_PI/360.0);
        nconstant = nearplane*tan(fov*M_PI/360.0);
        fconstant = farplane*tan(fov*M_PI/360.0);
        recalcModelView = true;
        break;
    case SDLK_1:
        if(smoothshade==2)
        {
            thrangdeg -= 5.0;
            if(thrangdeg<5.0) thrangdeg = 5.0;
            else
            {
                mesh_free_face_normals2();
                mfc1 = mesh_calc_face_normals2(m1, thrangdeg);
                needRedraw = true;
                if(displayListInited)
                {
                    glDeleteLists(indx[0], 1);
                    glDeleteLists(indx[1], 1);
                }
                displayListInited = false;
            }
        }
        break;
    case SDLK_2:
        if(smoothshade==2)
        {
            thrangdeg += 5.0;
            if(thrangdeg>180.0) thrangdeg = 180.0;
            else
            {
                mesh_free_face_normals2();
                mfc1 = mesh_calc_face_normals2(m1, thrangdeg);
                needRedraw = true;
                if(displayListInited)
                {
                    glDeleteLists(indx[0], 1);
                    glDeleteLists(indx[1], 1);
                }
                displayListInited = false;
            }
        }
        break;
    case SDLK_u:
        spec += 0.05;
        if(spec>1.0) spec = 1.0;
        else
        {
            spec_color[0] = spec;
            spec_color[1] = spec;
            spec_color[2] = spec;
            needRedraw = true;
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
        }
        break;
    case SDLK_j:
        spec -= 0.05;
        if(spec<0.0) spec = 0.0;
        {
            spec_color[0] = spec;
            spec_color[1] = spec;
            spec_color[2] = spec;
            needRedraw = true;
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
        }
        break;
    case SDLK_k:
        shininess -= 5;
        if(shininess<5) shininess = 5;
        else
        {
            needRedraw = true;
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
        }
        break;
    case SDLK_i:
        shininess += 5;
        if(shininess>100) shininess = 100;
        else
        {
            needRedraw = true;
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
        }
        break;
    case SDLK_v:
        wireframe = !wireframe;
        if(pcloud)
        {
            pcloud = false;
            if(displayListInited)
            {
                glDeleteLists(indx[0], 1);
                glDeleteLists(indx[1], 1);
            }
            displayListInited = false;
        }
        if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        needRedraw = true;
        break;
    case SDLK_c:
        pcloud = false;
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        ++smoothshade;
        if(smoothshade>2) smoothshade = 0;
        if(smoothshade>0) glShadeModel(GL_SMOOTH);
        else glShadeModel(GL_FLAT);
        if(smoothshade==2 && mfc1 ==NULL)
        {
            mfc1 = mesh_calc_face_normals2(m1, thrangdeg);
        }
        displayListInited = false;
        needRedraw = true;
        break;
    case SDLK_y:
        pcloud = !pcloud;
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        displayListInited = false;
        needRedraw = true;
        break;
    case SDLK_BACKSLASH:
        csel = !csel;
        break;
    case SDLK_r:
        if(csel)
        {
            clear_col[0] +=0.05;
            if(clear_col[0]>1.0) clear_col[0] = 1.0;
            glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        }
        else
        {
            light_dif[0] +=0.05;
            if(light_dif[0]>1.0) light_dif[0] = 1.0;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
        }
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        needRedraw = true;
        break;
    case SDLK_g:
        if(csel)
        {
            clear_col[1] +=0.05;
            if(clear_col[1]>1.0) clear_col[1] = 1.0;
            glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        }
        else
        {
            light_dif[1] +=0.05;
            if(light_dif[1]>1.0) light_dif[1] = 1.0;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
        }
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        needRedraw = true;
        break;
    case SDLK_b:
        if(csel)
        {
            clear_col[2] +=0.05;
            if(clear_col[2]>1.0) clear_col[2] = 1.0;
            glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        }
        else
        {
            light_dif[2] +=0.05;
            if(light_dif[2]>1.0) light_dif[2] = 1.0;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
        }
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        needRedraw = true;
        break;
    case SDLK_t:
        if(csel)
        {
            clear_col[0] -=0.05;
            if(clear_col[0]<0.0) clear_col[0] = 0.0;
            glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        }
        else
        {
            light_dif[0] -=0.05;
            if(light_dif[0]<0.0) light_dif[0] = 0.0;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
        }
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        needRedraw = true;
        break;
    case SDLK_h:
        if(csel)
        {
            clear_col[1] -=0.05;
            if(clear_col[1]<0.0) clear_col[1] = 0.0;
            glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        }
        else
        {
            light_dif[1] -=0.05;
            if(light_dif[1]<0.0) light_dif[1] = 0.0;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
        }
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        needRedraw = true;
        break;
    case SDLK_n:
        if(csel)
        {
            clear_col[2] -=0.05;
            if(clear_col[2]<0.0) clear_col[2] = 0.0;
            glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        }
        else
        {
            light_dif[2] -=0.05;
            if(light_dif[2]<0.0) light_dif[2] = 0.0;
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
        }
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        needRedraw = true;
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
    case SDLK_e:
        lighting = !lighting;
        if(lighting) glEnable(GL_LIGHTING);
        else glDisable(GL_LIGHTING);
        needRedraw = true;
        break;
    case SDLK_LEFTBRACKET:
        refdepth = refdepth-0.05;
        if(refdepth<-3.0) refdepth = -3.0;
        recalcModelView = true;
        needRedraw = true;
        break;
    case SDLK_RIGHTBRACKET:
        refdepth = refdepth+0.05;
        if(refdepth>-1.0) refdepth = -1.0;
        recalcModelView = true;
        needRedraw = true;
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
    case SDLK_SEMICOLON:
        transpar = !transpar;
        if(transpar) clear_col[3] = 0.0;
        else clear_col[3] = 1.0;
        glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
        needRedraw = true;
        break;
    case SDLK_TAB:
        show_minfo = !show_minfo;
        if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
        needRedraw = true;
        break;
    case SDLK_COMMA:
        show_axis = !show_axis;
        if(displayListInited)
        {
            glDeleteLists(indx[0], 1);
            glDeleteLists(indx[1], 1);
        }
        displayListInited = false;
        needRedraw = true;
        break;
    }
}

FLOATDATA project(FLOATDATA r, FLOATDATA x, FLOATDATA y)
{
    DEBUG_STDOUT("PROJECT\n")
    FLOATDATA d, t, z;
    d = std::sqrt(x*x+y*y);
    if(d<r*0.70710678118654752440) z = std::sqrt(r*r-d*d);
    else
    {
        t = r/1.41421356237309504880;
        z = t*t/d;
    }
    return z;
}

void track_pointer(int type, int x, int y)
{
    DEBUG_STDOUT("TRACK_POINTER\n")
    switch(type)
    {
    case 1:
        trk.x = (static_cast<FLOATDATA>(x)-0.5*width)/height;
        trk.y = 0.5-static_cast<FLOATDATA>(y)/height;
        break;
    case 2:
        mesh_vector3 p1, p2, d;
        p1 = trk;
        p2.x = (static_cast<FLOATDATA>(x)-0.5*width)/height;
        p2.y = 0.5-static_cast<FLOATDATA>(y)/height;
        trk = p2;
        if(butt==1)
        {
            quat<FLOATDATA> q;
            mesh_vector3 a;
            FLOATDATA t;
            p1.z = project(TRACKBALLSIZE, p1.x, p1.y);
            p2.z = project(TRACKBALLSIZE, p2.x, p2.y);
            mesh_cross_normal(&p1, &p2, &a);
            d.x = p1.x-p2.x;
            d.y = p1.y-p2.y;
            d.z = p1.z-p2.z;
            t = std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z)/(2.0*TRACKBALLSIZE);
            if(t>1.0) t = 1.0;
            else if(t<-1.0) t = -1.0;
            phi = 2.0*asin(t);
            q.fromangleaxis(a.x, a.y, a.z, phi);
            curr_rot = q*curr_rot;
        }
        else if(butt==2)
        {
            if(objcentric)
            {
                pos.z -= p2.y-p1.y;
            }
            else
            {
                p2.z = p2.y-p1.y;
                p2.x = 0.0;
                p2.y = 0.0;
                p1 = p2;
                apply_quat_inv(curr_rot, &p2, &p1);

                pos.x -= p1.x;
                pos.y -= p1.y;
                pos.z -= p1.z;
            }
            posconstant = pos.z*tan(fov*M_PI/360.0);
            nconstant = nearplane*tan(fov*M_PI/360.0);
            fconstant = farplane*tan(fov*M_PI/360.0);
        }
        else if(butt==3)
        {
            if(objcentric)
            {
                pos.x += p2.x-p1.x;
                pos.y += p2.y-p1.y;
            }
            else
            {
                p2.x = p2.x-p1.x;
                p2.y = p2.y-p1.y;
                p2.z = 0.0;
                p1 = p2;
                apply_quat_inv(curr_rot, &p2, &p1);
                pos.x += p1.x;
                pos.y += p1.y;
                pos.z += p1.z;
            }
        }
        recalcModelView = true;
        needRedraw = true;
    }
}

void glstereo_update(float bline, float rfdepth, float tfov, float nplane, float fplane, float aratio, int type)
{
    DEBUG_STDOUT("GLSTEREO_UPDATE\n")
    float top, bottom, left, right, a, b, c;
    if(type==GLSTEREO_MONO||bline<0.00001f)
    {
        sv->mono = 1;
        bline = 0.0f;
    }
    else sv->mono = 0;
    sv->aratio = aratio;
    sv->baseline = bline;
    sv->fov = tfov*GLSTEREO_PI/180.0;
    sv->fplane = fplane;
    sv->nplane = nplane;
    sv->refdepth = rfdepth;
    top = nplane*tanf(sv->fov/2);
    bottom  = -top;
    a = aratio*tanf(sv->fov/2)*refdepth;
    b = a-bline/2;
    c = a+bline/2;

    left = -b*nplane/rfdepth;
    right = c*nplane/rfdepth;
    __glstereo_set_frustum(sv->lfrustum, left, right, top, bottom, nplane, fplane);

    left = -c*nplane/rfdepth;
    right = b*nplane/rfdepth;
    __glstereo_set_frustum(sv->rfrustum, left, right, top, bottom, nplane, fplane);

    switch(type)
    {
    case GLSTEREO_RED_CYAN:
        sv->lcmask[0] = GL_TRUE;
        sv->lcmask[1] = GL_FALSE;
        sv->lcmask[2] = GL_FALSE;
        sv->lcmask[3] = GL_FALSE;

        sv->rcmask[0] = GL_FALSE;
        sv->rcmask[1] = GL_TRUE;
        sv->rcmask[2] = GL_TRUE;
        sv->rcmask[3] = GL_FALSE;
        break;

    case GLSTEREO_GREEN_MAGENTA:
        sv->lcmask[0] = GL_FALSE;
        sv->lcmask[1] = GL_TRUE;
        sv->lcmask[2] = GL_FALSE;
        sv->lcmask[3] = GL_FALSE;

        sv->rcmask[0] = GL_TRUE;
        sv->rcmask[1] = GL_FALSE;
        sv->rcmask[2] = GL_TRUE;
        sv->rcmask[3] = GL_FALSE;
        break;

    case GLSTEREO_MONO:
        sv->lcmask[0] = GL_TRUE;
        sv->lcmask[1] = GL_TRUE;
        sv->lcmask[2] = GL_TRUE;
        sv->lcmask[3] = GL_TRUE;

        sv->rcmask[0] = GL_FALSE;
        sv->rcmask[1] = GL_FALSE;
        sv->rcmask[2] = GL_FALSE;
        sv->rcmask[3] = GL_FALSE;
        break;

    default:
        glstereo_error(GLSTEREO_ERROR_TYPE);
    }
}

void update()
{
    DEBUG_STDOUT("UPDATE\n")
    mesh_vector3 ang;
    FLOATDATA theta;
    glViewport(0, 0, width, height);
    if(stereo==false)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fov, width/static_cast<FLOATDATA>(height), nearplane, farplane);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if(objcentric)
        {
            glTranslatef(pos.x+pos1.x, pos.y+pos1.y, pos.z+pos1.z);
            curr_rot.toangleaxis(&ang.x, &theta);
            glRotated(theta*(180.0/M_PI), ang.x, ang.y, ang.z);
            if(show_rc)
            {
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                mesh_draw_mesh(m_rc);
            }
        }
        else
        {
            glTranslatef(pos1.x, pos1.y, pos1.z);
            curr_rot.toangleaxis(&ang.x, &theta);
            glRotated(theta*(180.0/M_PI), ang.x, ang.y, ang.z);
            if(show_rc)
            {
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                mesh_draw_mesh(m_rc);
            }
            glTranslatef(pos.x, pos.y, pos.z);
        }

    }
    else
    {
        glstereo_update(baseline, refdepth, fov, nearplane, farplane, width/static_cast<FLOATDATA>(height), stereotype);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if(objcentric)
        {
            glTranslatef(pos.x+pos1.x, pos.y+pos1.y, pos.z+pos1.z);
            curr_rot.toangleaxis(&ang.x, &theta);
            glRotated(theta*(180.0/M_PI), ang.x, ang.y, ang.z);
            if(show_rc)
            {
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                //glstereo_left(sv);
                //mesh_draw_mesh(m_rc);
                //glstereo_right(sv);
                //mesh_draw_mesh(m_rc);
                //glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
            }
        }
        else
        {
            glTranslatef(pos1.x, pos1.y, pos1.z);
            curr_rot.toangleaxis(&ang.x, &theta);
            glRotated(theta*(180.0/M_PI), ang.x, ang.y, ang.z);
            if(show_rc)
            {
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            }
            glTranslatef(pos.x, pos.y, pos.z);
        }
        glGetFloatv(GL_MODELVIEW_MATRIX, sv->modelview);
    }
    recalcModelView = false;
    needRedraw = true;
}

void render()
{
    DEBUG_STDOUT("RENDER\n")
    if(lighting) glEnable(GL_LIGHTING);
    if(!show_rc ||stereo) glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(displayListInited)
    {
        DEBUG_STDOUT("EXEC_DLIST\n")
        if(stereo==false)
        {
            glCallList(indx[0]);
            if(show_axis)
            {
                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glCallList(indx[1]);
                if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                if(lighting) glEnable(GL_LIGHTING);
                else glDisable(GL_LIGHTING);
            }
        }
        else
        {
            glstereo_left(sv);
            glCallList(indx[0]);
            if(show_axis)
            {
                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glCallList(indx[1]);
                if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                if(lighting) glEnable(GL_LIGHTING);
                else glDisable(GL_LIGHTING);
            }
            glstereo_right(sv);
            glCallList(indx[0]);
            if(show_axis)
            {
                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glCallList(indx[1]);
                if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                if(lighting) glEnable(GL_LIGHTING);
                else glDisable(GL_LIGHTING);
            }
            glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        }
    }
    else
    {
        DEBUG_STDOUT("GENERATE DLIST\n")
        indx[0] = glGenLists(1);
        if(stereo)
        {
            glstereo_left(sv);
        }
        glNewList(indx[0], GL_COMPILE_AND_EXECUTE);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec_color);
        glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        if(!pcloud)
        {
            if(smoothshade==2) mesh_draw_mesh_smooth2(m1, mfc1);
            else if(smoothshade==1) mesh_draw_mesh_smooth(m1);
            else mesh_draw_mesh(m1);
        }
        else
        {
            mesh_draw_point_cloud(m1);
            if(dside&&lighting&&m1->is_vnormals)
            {
                if(mivn==NULL)
                {
                    mivn = mesh_create_mesh_new();
                    if(mivn!=NULL)
                    {
                        mivn->vertices = (MESH_VERTEX)malloc(sizeof(mesh_vertex)*m1->num_vertices);
                        mivn->is_loaded = 1;
                        if(mivn->vertices!=NULL)
                        {
                            mivn->is_vertices = 1;
                            mivn->num_vertices = m1->num_vertices;
                            mivn->vnormals = (MESH_NORMAL)malloc(sizeof(mesh_normal)*m1->num_vertices);

                            if(mivn->vnormals!=NULL)
                            {
                                mivn->is_vnormals = 1;
                                #pragma omp parallel for
                                for(INTDATA i=0; i<m1->num_vertices; ++i)
                                {
                                    mivn->vnormals[i].x = -m1->vnormals[i].x;
                                    mivn->vnormals[i].y = -m1->vnormals[i].y;
                                    mivn->vnormals[i].z = -m1->vnormals[i].z;
                                    mivn->vertices[i].x = m1->vertices[i].x +0.0025*mivn->vnormals[i].x;
                                    mivn->vertices[i].y = m1->vertices[i].y +0.0025*mivn->vnormals[i].y;
                                    mivn->vertices[i].z = m1->vertices[i].z +0.0025*mivn->vnormals[i].z;
                                }
                                if(m1->is_vcolors)
                                {
                                    mivn->vcolors = (MESH_COLOR)malloc(sizeof(mesh_color)*m1->num_vertices);
                                    if(mivn->vcolors!=NULL)
                                    {
                                        mivn->is_vcolors = 1;
                                        #pragma omp parallel for
                                        for(INTDATA i=0; i<m1->num_vertices; ++i)
                                        {
                                            mivn->vcolors[i].r = m1->vcolors[i].r;
                                            mivn->vcolors[i].g = m1->vcolors[i].g;
                                            mivn->vcolors[i].b = m1->vcolors[i].b;
                                            mivn->vcolors[i].a = m1->vcolors[i].a;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                mesh_draw_point_cloud(mivn);
            }
        }

        if(vnormd&&m1->is_vnormals)
        {
            glDisable(GL_LIGHTING);
            glLineWidth(psize);
            glBegin(GL_LINES);
            glColor3f(1.0f, 0.5f, 0.2f);
            mesh_vertex ntip;
            for(INTDATA i=0; i<m1->num_vertices; ++i)
            {
                ntip.x = m1->vertices[i].x+m1->vnormals[i].x*vnormscale;
                ntip.y = m1->vertices[i].y+m1->vnormals[i].y*vnormscale;
                ntip.z = m1->vertices[i].z+m1->vnormals[i].z*vnormscale;
                glVertex3f(m1->vertices[i].x, m1->vertices[i].y, m1->vertices[i].z);
                glVertex3f(ntip.x, ntip.y, ntip.z);
            }
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        if(fnormd&&m1->is_fnormals)
        {
            glDisable(GL_LIGHTING);
            glLineWidth(psize);
            glBegin(GL_LINES);
            glColor3f(0.2f, 1.0f, 0.5f);
            mesh_vertex nbase, ntip;
            MESH_VERTEX mv = m1->vertices;
            if(m1->is_trimesh)
            {
                for(INTDATA i=0; i<m1->num_faces; ++i)
                {
                    INTDATA* cf = m1->faces[i].vertices;
                    nbase.x = 0.33333333*(mv[cf[0]].x+mv[cf[1]].x+mv[cf[2]].x);
                    nbase.y = 0.33333333*(mv[cf[0]].y+mv[cf[1]].y+mv[cf[2]].y);
                    nbase.z = 0.33333333*(mv[cf[0]].z+mv[cf[1]].z+mv[cf[2]].z);

                    ntip.x = nbase.x+m1->fnormals[i].x*vnormscale;
                    ntip.y = nbase.y+m1->fnormals[i].y*vnormscale;
                    ntip.z = nbase.z+m1->fnormals[i].z*vnormscale;
                    glVertex3f(nbase.x, nbase.y, nbase.z);
                    glVertex3f(ntip.x, ntip.y, ntip.z);
                }
            }
            else
            {
                for(INTDATA i=0; i<m1->num_faces; ++i)
                {
                    MESH_FACE cf = m1->faces+i;
                    nbase.x = 0.0;
                    nbase.y = 0.0;
                    nbase.z = 0.0;
                    for(INTDATA j=0; j<cf->num_vertices; ++j)
                    {
                        nbase.x += mv[cf->vertices[j]].x;
                        nbase.y += mv[cf->vertices[j]].y;
                        nbase.z += mv[cf->vertices[j]].z;
                    }
                    if(cf->num_vertices>0)
                    {
                        nbase.x /= cf->num_vertices;
                        nbase.y /= cf->num_vertices;
                        nbase.z /= cf->num_vertices;
                    }
                    ntip.x = nbase.x+m1->fnormals[i].x*vnormscale;
                    ntip.y = nbase.y+m1->fnormals[i].y*vnormscale;
                    ntip.z = nbase.z+m1->fnormals[i].z*vnormscale;
                    glVertex3f(nbase.x, nbase.y, nbase.z);
                    glVertex3f(ntip.x, ntip.y, ntip.z);
                }
            }
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
        }

        glEndList();

        indx[1] = glGenLists(1);
        glNewList(indx[1], GL_COMPILE_AND_EXECUTE);
        if(show_axis)
        {
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            mesh_draw_mesh(maxis);
            if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            if(lighting) glEnable(GL_LIGHTING);
            else glDisable(GL_LIGHTING);
        }
        glEndList();
        displayListInited = true;
        if(stereo)
        {
            glstereo_right(sv);
            glCallList(indx[0]);
            if(show_axis)
            {
                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glCallList(indx[1]);
                if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                if(lighting) glEnable(GL_LIGHTING);
                else glDisable(GL_LIGHTING);
            }
            glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        }
    }
    if(text_ok &&show_minfo)
    {
        if(wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            render_text(0, -srcrect.y);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else render_text(0, -srcrect.y);
    }
    if(enablecanvas)
    {
        mvcanvas_resize(width, height);
        mvcanvas_display();
        update();
    }
}

void close()
{
    DEBUG_STDOUT("CLOSE\n")
    mesh_free_face_normals2();
    if(colour_generated)
    {
        if(has_prev_vcolors) free(scalar_colors);
        else
        {
            m1->is_vcolors = 0;
            if(!use_dcolors) free(scalar_colors);
        }
    }
    mesh_free_mesh(m1);
    mesh_free_mesh(m_rc);
    if(mivn!=NULL) mesh_free_mesh(mivn);
    mesh_free_mesh(maxis);
    m1 = NULL;
    m_rc = NULL;
    maxis = NULL;
    if(text_ok)
    {
        glDeleteTextures(1, &vfbb_tex);
        SDL_FreeSurface(vfbb);
        TTF_CloseFont(font);
        TTF_Quit();
    }
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    if(displayListInited)
    {
        glDeleteLists(indx[0], 1);
        glDeleteLists(indx[1], 1);
    }
    glstereo_delete(sv);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    freopen("CON", "wt", stdout);
    freopen("CON", "wt", stderr);
    DEBUG_STDOUT("MAIN\n")
    get_exec_path(argv[0]);
    SDL_Event e;
    SDL_Keymod kmod;
    if(argc>1&&argv[1][0]=='-'&&argv[1][1]=='f')
    {
        --argc;
        ++argv;
        init_fs = true;
    }
    if(argc>1&&argv[1][0]=='-'&&argv[1][1]=='c')
    {
        configure = true;
        configure_fname = argv[2];
        --argc;
        --argc;
        ++argv;
        ++argv;
    }
    if(argc<2) fatalerror("Filename not given.");
    init();
    load_files(argc-1, argv+1);
    if(configure)
    {
        load_infra(configure_fname);
    }

    SDL_StartTextInput();

    while(!quit)
    {
        if(SDL_WaitEvent(&e)!=0)
        {
            switch(e.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                handle_keys(e.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(enablecanvas)
                {
                    mvcanvas_mouse(SDL_MOUSEBUTTONDOWN, e.button.x, e.button.y);
                    needRedraw = true;
                    break;
                }
                switch(e.button.button)
                {
                case SDL_BUTTON_LEFT:
                    butt = 1;
                    track_pointer(1, e.button.x, e.button.y);
                    tracking = true;
                    needRedraw = true;
                    break;
                case SDL_BUTTON_MIDDLE:
                    butt = 2;
                    track_pointer(1, e.button.x, e.button.y);
                    tracking = true;
                    needRedraw = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    butt = 3;
                    track_pointer(1, e.button.x, e.button.y);
                    tracking = true;
                    needRedraw = true;
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if(enablecanvas)
                {
                    mvcanvas_mouse(SDL_MOUSEBUTTONUP, e.button.x, e.button.y);
                    needRedraw = true;
                    break;
                }
                tracking = false;
                butt = 0;
                needRedraw = true;
                break;
            case SDL_MOUSEMOTION:
                if(enablecanvas)
                {
                    mvcanvas_motion(e.button.x, e.button.y);
                    needRedraw = true;
                    break;
                }
                if(tracking)
                {
                    track_pointer(2, e.button.x, e.button.y);
                    needRedraw = true;
                }
                break;
            case SDL_MOUSEWHEEL:
                if(!tracking)
                {
                    kmod = SDL_GetModState();
                    if(kmod==KMOD_LSHIFT||kmod==KMOD_RSHIFT)
                    {
                        if(e.wheel.y>0)
                        {
                            fov += 1.0;
                            if(fov>90.0) fov = 90.0;
                            FLOATDATA sc = 1.0/tan(fov*M_PI/360.0);
                            pos.z = posconstant*sc;
                            nearplane = nconstant*sc;
                            farplane = fconstant*sc;
                            recalcModelView = true;
                        }
                        else
                        {
                            fov -= 1.0;
                            if(fov<2.0) fov = 2.0;
                            FLOATDATA sc = 1.0/tan(fov*M_PI/360.0);
                            pos.z = posconstant*sc;
                            nearplane = nconstant*sc;
                            farplane = fconstant*sc;
                            recalcModelView = true;
                        }
                    }
                }
                break;
            case SDL_WINDOWEVENT:
                if(e.window.event==SDL_WINDOWEVENT_EXPOSED) needRedraw = true;
                else if(e.window.event==SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    width = e.window.data1;
                    height= e.window.data2;
                    if(text_ok && show_minfo) generate_text(m1, &minm, &maxm);
                    if(enablecanvas)
                    {
                        mvcanvas_resize(width, height);
                    }
                    recalcModelView = true;
                }
                break;
            default:
                break;
            }
        }
        if(recalcModelView) update();
        if(needRedraw)
        {
            render();
            SDL_GL_SwapWindow(gWindow);
            needRedraw = false;
        }
    }
    SDL_StopTextInput();
    close();
    return 0;
}

