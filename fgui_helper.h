/*
    FemtoGUI

    fgui_helper.h:
        Internal helper include file

    copyright:
              Copyright (c) 2006-2008 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:
        Don't include this file in your application code
*/
#ifndef FGUI_HELPER_H
#define FGUI_HELPER_H

#ifndef FGUI_CODE
#error Do not include this file!
#endif

typedef struct {
    /* Font settings */
    struct {
        /* Character size */
        unsigned char h, w, bytew;
        /* Points to the data location */
        unsigned char *pixeldata;
    } font;

    /* Foreground color (background is the inversion of this) */
    unsigned char fgcolor;

    /* Points to the pixeldata buffer */
    unsigned char *fb;
} fgui_t;

unsigned int bitstobytesup(const unsigned int bits);
unsigned int bitstobytesdown(const unsigned int bits);
void copypixeldata(int x, int y, const unsigned char *pixeldata, int w, int h, unsigned int bytew);

#endif /* FGUI_HELPER_H */