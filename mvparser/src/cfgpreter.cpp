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
 * @file cfgpreter.cpp
 * @author Sk. Mohammadul Haque
 * @version 2.0.1.0
 * @copyright
 * Copyright (c) 2019-2022 Sk. Mohammadul Haque.
 * @brief This file contains definitions of CFGPreter structures and functions.
 */

#include "../include/cfgpreter.h"
#include <iostream>
#include <cstring>

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

std::array<std::pair<std::string, std::pair<int, int>>,NUM_CFGWORDS> cfgstrings;

bool cfgstrings_ready = false;
void mvcfg_init_cfgstrings();
int mvcfg_read_word(FILEPOINTER fp, char *c_word);
int mvcfg_getmvcgfnum(const char *str);
mvcfgword mvcfg_parse_line_program_file(FILEPOINTER fp);

void mvcfg_init_cfgstrings()
{
    DEBUG_STDOUT("MVCFG_INIT_CFGSTRINGS\n")
    cfgstrings[0] = {"",{0,0}};
    cfgstrings[CMD_PREV_MODEL] = {"prev_model",{0,0}};
    cfgstrings[CMD_NEXT_MODEL] = {"next_model",{0,0}};
    cfgstrings[CMD_EXIT] = {"exit",{0,0}};
    cfgstrings[CMD_HEIGHT] = {"height",{1,0}};
    cfgstrings[CMD_WIDTH] = {"width",{1,0}};
    cfgstrings[CMD_SHININESS] = {"shininess",{1,0}};
    cfgstrings[CMD_SPEC] = {"spec",{1,0}};
    cfgstrings[CMD_FOV] = {"fov",{1,0}};
    cfgstrings[CMD_PSIZE] = {"psize",{1,0}};
    cfgstrings[CMD_PCLOUD] = {"pcloud",{1,0}};
    cfgstrings[CMD_WIREFRAME] = {"wireframe",{1,0}};
    cfgstrings[CMD_AXIS] = {"axis",{1,0}};
    cfgstrings[CMD_LIGHTING] = {"lighting",{1,0}};
    cfgstrings[CMD_VNORM] = {"vnorm",{1,0}};
    cfgstrings[CMD_FNORM] = {"fnorm",{1,0}};
    cfgstrings[CMD_SEMANTICS] = {"semantics",{1,0}};
    cfgstrings[CMD_TRANSBG] = {"transbg",{1,0}};
    cfgstrings[CMD_INFO] = {"info",{1,0}};
    cfgstrings[CMD_RENDER_TYPE] = {"render_type",{1,0}};
    cfgstrings[CMD_BG_COLOUR] = {"bg_colour",{3,0}};
    cfgstrings[CMD_LT_COLOUR] = {"lt_colour",{3,0}};
    cfgstrings[CMD_POS] = {"pos",{3,0}};
    cfgstrings[CMD_ROT] = {"rot",{3,0}};
    cfgstrings[CMD_SAVE] = {"save",{0,1}};
    cfgstrings[CMD_TEXT] = {"text",{2,1}};
    cfgstrings_ready = true;
}

mvcfg cfgs;

int mvcfg_read_word(FILEPOINTER fp, char *c_word)
{
    DEBUG_STDOUT("MVCFG_READ_WORD\n")
    int flag = 0, t = 0, comment_flag = 0;
    int ch = 0;
    while((flag<3) && ((ch = (char)getc(fp))!=EOF))/*no need for state 3 to be corrected*/
    {
        if ((ch =='\v') || (ch =='\r') || (ch =='\n') || (ch =='\t') || isspace(ch) || (ch == ',') || (ch == '!') || (ch == '(') || (ch == ')') || (ch == '{') || (ch == '}') || (ch == '[') || (ch == ']'))
        {
            if(flag!=0) flag = 2;
        }
        else if(flag<2)
        {
            flag = 1;
            if(ch=='%') /* comment line to skip */
            {
                flag = 4;
                comment_flag = 1;
            }
            c_word[t++] = ch;
        }
        else if(flag==2) flag = 3; /* reached next word */ /*to be corrected for deleting state 3*/
    }
    c_word[t] = '\0';
    if(flag==4) /* skip remaining part of the line */
    {
         while((comment_flag==1) && ((ch = (char)getc(fp))!=EOF))/*no need for state 3 to be corrected*/
        {
            if (ch =='\r' || ch =='\n')
            {
                comment_flag = 0;
            }
        }
        if(ch!=EOF)
        {
            ungetc(ch, fp);
            return MVCFG_RWRV_COMMENT;
        }
        else return MVCFG_RWRV_COMMENT;
    }
    if(ch!=EOF)
    {
        ungetc(ch, fp);
        return MVCFG_RWRV_OKAY;
    }
    else return MVCFG_RWRV_EOF;
}

int mvcfg_getcodenum(const std::string& str)
{
    DEBUG_STDOUT("MVCFG_GETCODENUM\n")
    int c = 0, i;
    for(i=0; i<NUM_CFGWORDS; i++)
    {
        if(str.compare(cfgstrings[i].first)==0)
        {
            c = i;
            break;
        }
    }
    return c;
}

mvcfgword mvcfg_parse_line_program_file(FILEPOINTER fp)
{
    DEBUG_STDOUT("MVCFG_PARSE_LINE_PROGRAM_FILE\n")
    int c = 0, iv = 0, rv = 0;
    float fv;
    char c_word[512];
    char txtbuf[TXTMAX_SZ] = {0};
    mvcfgword pc;
    pc.codenum = CMD_ERROR;
    pc.iscomment = 0;
    rv = mvcfg_read_word(fp, c_word);
    if(rv==MVCFG_RWRV_COMMENT)
    {
        pc.iscomment = 1;
        return pc;
    }
    c = mvcfg_getcodenum(c_word);
    if(c==CMD_EOF)
    {
        pc.codenum = CMD_EOF;
        return pc;
    }
    else
    {
        // below SMH removed this block, as it invalidates code
        // if eof reached with valid complete code
//        if(rv==MVCFG_RWRV_EOF) /* eof reached already*/
//        {
//            pc.codenum = CMD_EOF;
//            return pc;
//        }
        switch(c)
        {
        case CMD_PREV_MODEL: /* prev_model */
        case CMD_NEXT_MODEL: /* next_model */
        case CMD_EXIT: /* exit */
            pc.codenum = CMD_EXIT;
            break;
        case CMD_HEIGHT: /* height */
        case CMD_WIDTH: /* width */
        case CMD_SHININESS: /* shininess */
        case CMD_SPEC: /* spec */
        case CMD_FOV: /* fov */
        case CMD_PSIZE: /* psize */
            rv = mvcfg_read_word(fp, c_word);
            fv = strtod(c_word, NULL);
            if(fv<0)
            {
                pc.codenum = CMD_ERROR;
                return pc;
            }
            pc.codenum = c;
            pc.codevals.emplace_back(fv);
            break;
        case CMD_PCLOUD: /* pcloud */
        case CMD_WIREFRAME: /* wireframe */
        case CMD_AXIS: /* axis */
        case CMD_LIGHTING: /* lighting */
        case CMD_VNORM: /* vnorm */
        case CMD_FNORM: /* fnorm */
        case CMD_SEMANTICS: /* semantics */
        case CMD_TRANSBG: /* transbg */
        case CMD_INFO: /* info */
            rv = mvcfg_read_word(fp, c_word);
            iv = strtol(c_word, NULL, 0);
            if(iv<0 ||iv>1)
            {
                pc.codenum = CMD_ERROR;
                return pc;
            }
            pc.codenum = c;
            pc.codevals.emplace_back(iv);
            break;
        case CMD_RENDER_TYPE: /* render_type */
            rv = mvcfg_read_word(fp, c_word);
            iv = strtol(c_word, NULL, 0);
            if(iv<0 ||iv>2)
            {
                pc.codenum = CMD_ERROR;
                break;
            }
            pc.codenum = c;
            pc.codevals.emplace_back(iv);
            break;
        case CMD_BG_COLOUR: /* bg_colour */
        case CMD_LT_COLOUR: /* lt_colour */
        case CMD_POS: /* pos */
        case CMD_ROT: /* rot */
            rv = mvcfg_read_word(fp, c_word);
            if(rv==MVCFG_RWRV_OKAY)
            {
                fv = strtod(c_word, NULL);
                pc.codevals.emplace_back(fv);
                rv = mvcfg_read_word(fp, c_word);
                if(rv==MVCFG_RWRV_OKAY)
                {
                    fv = strtod(c_word, NULL);
                    pc.codevals.emplace_back(fv);
                    rv = mvcfg_read_word(fp, c_word);
                    if(rv==MVCFG_RWRV_OKAY)
                    {
                        fv = strtod(c_word, NULL);
                        pc.codevals.emplace_back(fv);
                        pc.codenum = c;
                        break;
                    }
                }
            }
            pc.codevals.clear();
            pc.codenum = CMD_ERROR;
            break;
        case CMD_SAVE: /* save */
            if(fgets(txtbuf, TXTMAX_SZ-1, fp)!=NULL)
            {
                txtbuf[strcspn(txtbuf, "\n")] = 0;
                txtbuf[strcspn(txtbuf, "\r")] = 0;
                pc.codetxts.emplace_back(txtbuf);
                pc.codenum = CMD_SAVE;
                break;
            }
            pc.codevals.clear();
            pc.codenum = CMD_ERROR;
            break;
        case CMD_TEXT: /* text */
            rv = mvcfg_read_word(fp, c_word);
            if(rv==MVCFG_RWRV_OKAY)
            {
                fv = strtod(c_word, NULL);
                pc.codevals.emplace_back(iv);
                rv = mvcfg_read_word(fp, c_word);
                if(rv==MVCFG_RWRV_OKAY)
                {
                    fv = strtod(c_word, NULL);
                    pc.codevals.emplace_back(iv);
                    if(fgets(txtbuf, TXTMAX_SZ-1, fp)!=NULL)
                    {
                        txtbuf[strcspn(txtbuf, "\n")] = 0;
                        txtbuf[strcspn(txtbuf, "\r")] = 0;
                        pc.codetxts.emplace_back(txtbuf);
                        pc.codenum = CMD_TEXT;
                        break;
                    }
                }
            }
            pc.codevals.clear();
            pc.codenum = CMD_ERROR;
            break;

        default:
            pc.codenum = CMD_ERROR; /* check later */
        }
    }
    return pc;
}

void mvcfg_load(const char *fname)
{
    DEBUG_STDOUT("MVCFG_LOAD\n")
    int flag = 0, k = 0;
    mvcfgword curr_cwd;
    FILEPOINTER fp = NULL;
    cfgs.allocated = 0;
    cfgs.cwrds.clear();

    if(!cfgstrings_ready) mvcfg_init_cfgstrings();

    if((fp = fopen(fname, "r"))!=NULL)
    {
        while(!flag)
        {
            curr_cwd = mvcfg_parse_line_program_file(fp);
            if(curr_cwd.iscomment==1) continue;
            if(curr_cwd.codenum==CMD_EOF)
            {
                /* eof */
                flag = 1;
            }
            else if(curr_cwd.codenum!=CMD_ERROR)
            {
                ++k;
                cfgs.cwrds.emplace_back(curr_cwd);
                cfgs.allocated = 1;
            }
            else if(curr_cwd.codenum==CMD_ERROR)
            {
                /* error */
                flag = 1;
                if(cfgs.allocated) cfgs.cwrds.clear();
                cfgs.allocated = 0;
            }
        }
        fclose(fp);
    }
}

void mvcfg_unload()
{
    DEBUG_STDOUT("MVCFG_UNLOAD\n")
    cfgs.cwrds.clear();
    cfgs.allocated = 0;
}

void mvcfg_print()
{
    DEBUG_STDOUT("MVCFG_PRINT\n")
    int numcfgs = cfgs.cwrds.size();
    std::cout<<"[[CFGS]]\n\tnum_cwrds: "<<numcfgs<<"\n";
    std::cout<<"------------------------------\n";
    for(int i=0; i<numcfgs; ++i)
    {
        mvcfgword& ccwrd = cfgs.cwrds[i];
        std::cout<<"["<<i<<"] code_num: "<<ccwrd.codenum<<" code_name: "<<cfgstrings[ccwrd.codenum].first<<"\n";
        std::cout<<"\tnum_val_args: "<<ccwrd.codevals.size()<<"\n\t\t";
        for(auto&ccval:ccwrd.codevals)
        {
            std::cout<<ccval<<" ";
        }
        std::cout<<"\n";
        std::cout<<"\tnum_txt_args: "<<ccwrd.codetxts.size()<<"\n\t\t";
        for(auto&cctxt:ccwrd.codetxts)
        {
            std::cout<<cctxt<<"\n";
        }
        std::cout<<"\n";
        std::cout<<"------------------------------\n";
    }
}


