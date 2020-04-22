/*
    FemtoGUI

    fgui_clearregion.c:
        clearregion function

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

void fgui_clearregion(int x, int y, unsigned int w, unsigned int h)
/*
  Clear a region of the framebuffer at location x,y, with width 'w' and height 'h'
*/
{
    unsigned int cw,ch,bytestart;
    unsigned char bitmask;

    /* Sanity check */
    if(x > FGUI_SCR_W || y > FGUI_SCR_H) {
        return;
    }
    if(x < 0) {
        x = 0;
    }
    if(y < 0) {
        y = 0;
    }
    if(x + w > FGUI_SCR_W) {
        w = FGUI_SCR_W - x;
    }
    if(y + h > FGUI_SCR_H) {
        h = FGUI_SCR_H - y;
    }

    if(x%8 > 0) {
        /* Region is not byte-aligned with the framebuffer, bitshifting is needed to get the first part */
        bitmask = (FGUI_BLACK>>(x%8));
        if(fgui.fgcolor == FGUI_BLACK) {
            for(ch=0;ch<h;ch++) {
                fgui.fb[((y+ch)*FGUI_SCR_BYTEW)+bitstobytesdown(x)] &= ~bitmask;
            }
        }
        else {
            for(ch=0;ch<h;ch++) {
                fgui.fb[((y+ch)*FGUI_SCR_BYTEW)+bitstobytesdown(x)] |= bitmask;
            }
        }
    }

    /* OK, now copy all complete bytes */
    for(ch=0;ch<h;ch++) {
        for(cw=bitstobytesup(x);cw<bitstobytesdown(x+w); cw++) {
            fgui.fb[((y+ch)*FGUI_SCR_BYTEW) + cw] = ~fgui.fgcolor;
        }
    }

    /* Right now all whole bytes of the region are cleared;
       if there are some bits left, we clear them here */
    if((x+w)%8) {
        bytestart = bitstobytesdown(x+w);
        bitmask = FGUI_BLACK<<(8-((x+w)%8));
        if(fgui.fgcolor == FGUI_BLACK) {
            for(ch=0;ch<h;ch++) {
                fgui.fb[((y+ch)*FGUI_SCR_BYTEW)+bytestart] &= ~bitmask;
            }
        }
        else {
            for(ch=0;ch<h;ch++) {
                fgui.fb[((y+ch)*FGUI_SCR_BYTEW)+bytestart] |= bitmask;
            }
        }
    }
}

void fgui_fillregion(int x, int y, unsigned int w, unsigned int h)
/*
  Fill a region of the framebuffer at location x,y, with width 'w' and height 'h'
*/
{
    fgui.fgcolor = ~fgui.fgcolor;

    fgui_clearregion(x, y, w, h);

    fgui.fgcolor = ~fgui.fgcolor;
}