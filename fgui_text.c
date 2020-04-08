/*
    FemtoGUI

    fgui_text.c:
        Text functions

    copyright:
              Copyright (c) 2006-2008 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:

*/
#include "fgui.h"

#define FGUI_CODE
#include "fgui_helper.h"

extern fgui_t fgui;

void fgui_setfont(const unsigned char *newfont)
/*
  Select the font to use. Must be called before using any of the other text
  functions.
  'newfont' must point to a valid font array. Elements in the array are:
   0    char width in bits (pixels)
   1    char height in bits (pixels)
   2-n  The actual pixel data
*/
{
    fgui.font.w = newfont[0];
    fgui.font.bytew = bitstobytesup(newfont[0]);
    fgui.font.h = newfont[1];
    fgui.font.pixeldata = (unsigned char *)&newfont[2];
}

unsigned char fgui_charheight()
/*
  Height of active font, in pixels
*/
{
    return fgui.font.h;
}

unsigned char fgui_charwidth()
/*
  Width of a single character from the active font, in pixels
*/
{
    return fgui.font.w;
}

unsigned int fgui_strlen(char *s)
/*
  Get length of given string, in pixels
*/
{
    int length=0;

    while(*s) {
        s++;
        length++;
    }

    return length * fgui.font.w;
}

void fgui_char(const int x, const int y, const char c)
/*
  Place a single character at given location
*/
{
    copypixeldata(x, y, &fgui.font.pixeldata[(c-32) * (fgui.font.h * fgui.font.bytew)], fgui.font.w, fgui.font.h, fgui.font.bytew, fgui.fgcolor);
}

void fgui_text(int x, const int y, const char *str)
/*
  Place a NULL terminated string at the given location
*/
{
    while(*str) {
        fgui_char(x,y,*str);
        x += fgui.font.w;
        str++;
    }
}
