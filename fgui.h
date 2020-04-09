/*
    FemtoGUI

    fgui.h:
       Main include file

    copyright:
              Copyright (c) 2006-2008 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

*/
#ifndef FGUI_H
#define FGUI_H

#include "fgui_config.h"

#define FGUI_VERSION "0.4"

#define FGUI_BLACK    0xFF
#define FGUI_WHITE    0x00

/* General functions */
char fgui_init(unsigned char *framebuffer);
void fgui_clear(void);
void fgui_clearregion(int x, int y, unsigned int w, unsigned int h);
void fgui_fillregion(int x, int y, unsigned int w, unsigned int h);
char fgui_refresh(void);
void fgui_setcolor(const unsigned char newcolor);
void fgui_invertcolor(void);

/* Pixel functions */
void fgui_pixelon(const unsigned int x, const unsigned int y);
void fgui_pixeloff(const unsigned int x, const unsigned int y);
void fgui_pixel(const unsigned int x, const unsigned int y, const unsigned char value);

/* Line functions */
void fgui_line(const unsigned int x0, const unsigned int y0, const unsigned int x1, const unsigned int y1);
void fgui_lineh(const unsigned int x, const unsigned int y, unsigned int l);
void fgui_linev(const unsigned int x, const unsigned int y, unsigned int l);

/* Sprite functions */
void fgui_sprite(const int x, const int y, const unsigned char *sprite);
int fgui_spritewidth(const unsigned char *sprite);
int fgui_spriteheight(const unsigned char *sprite);

/* Text functions */
void fgui_setfont(const unsigned char *font);
unsigned char fgui_charheight(void);
unsigned char fgui_charwidth(void);
unsigned int fgui_strlen(char *s);
void fgui_char(const int x, const int y, const char c);
void fgui_text(int x, const int y, const char *str);

#endif /* FGUI_H */
