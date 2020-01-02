/*
    FemtoGUI

    fgui.c:
        Main code

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

fgui_t fgui;

char fgui_init(unsigned char *framebuffer)
/*
  Set console parameters and initialise the screen by calling screen_init().
  Returncode is the result of screen_init(), which should be '1' on success.
*/
{
    fgui.fb = framebuffer;
    fgui.fgcolor = FGUI_BLACK;

    return screen_init();
}

void fgui_clear()
/*
  Clear framebuffer.
*/
{
    unsigned int i;

    /* clear the buffer */
    for(i=0;i<FGUI_FBSIZE;i++) {
        fgui.fb[i]=0;
    }
}

char fgui_refresh()
/*
  Update screen (as in, write contents of buffer to the screen).
  Returncode is the result of screen_update(), which should be '1' on success.
*/
{
    return screen_update(fgui.fb);
}

void fgui_setcolor(const unsigned char newcolor)
/*
  Set the foregroundcolor to use.
*/
{
    if(newcolor == FGUI_BLACK || newcolor == FGUI_WHITE) {
        fgui.fgcolor = newcolor;
    }
}

/*
  Pixel functions
*/

void fgui_pixelon(const unsigned int x, const unsigned int y)
/*
  Turn pixel at location x,y on (foreground color)
*/
{
    fgui_pixel(x,y,fgui.fgcolor);
}

void fgui_pixeloff(const unsigned int x, const unsigned int y)
/*
  Turn pixel at location x,y off (background color)
*/
{
    fgui_pixel(x,y,~fgui.fgcolor);
}

void fgui_pixel(const unsigned int x, const unsigned int y, const unsigned char value)
/*
  Set pixel at location x,y on (value == FGUI_BLACK) or off (value == FGUI_WHITE)
*/
{
    if(x < FGUI_SCR_W && y < FGUI_SCR_H) {
        if(value == FGUI_BLACK) {
            fgui.fb[y*FGUI_SCR_BYTEW + bitstobytesdown(x)] |= 1<<(7-(x%8));
        }
        else if(value == FGUI_WHITE) {
            fgui.fb[y*FGUI_SCR_BYTEW + bitstobytesdown(x)] &= ~(1<<(7-(x%8)));
        }
    }
}
