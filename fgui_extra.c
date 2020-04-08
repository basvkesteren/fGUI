/*
    FemtoGUI

    fgui_extra.c:
        Extra's

    copyright:
              Copyright (c) 2020 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:

*/
#include "fgui_extra.h"
#include "fgui.h"

#define FGUI_CODE
#include "fgui_helper.h"

extern fgui_t fgui;

static char *fgui_printbuffer;
static int fgui_printbufferlength;

void fgui_setprintbuffer(char* buffer, int bufferlength)
/*
  Set buffer (and length) to be used by fgui_print and fgui_printline
*/
{
    fgui_printbuffer = buffer;
    fgui_printbufferlength = bufferlength;
}

char fgui_print(int x, int y, char *fmt, ...)
/*
  Print text (printf format) at given x/y location. 
  Returns 1 on success, false otherwise
*/
{
    if(fgui_printbuffer && fgui_printbufferlength) {
        va_list argpointer;
        va_start(argpointer, fmt);

        FGUI_VSNPRINTF(fgui_printbuffer, fgui_printbufferlength, fmt, argpointer);
        fgui_text(x, y, fgui_printbuffer);

        va_end(argpointer);

        return 1;
    }
    return 0;
}

char fgui_printline(int line, fgui_linealignment_t alignment, char invert, char *fmt, ...)
/*
  Print a line of text at the given line. This'll clear the whole line before printing the text!
  Returns 1 on success, false otherwise
*/
{
    int x, y;

    if(fgui_printbuffer && fgui_printbufferlength) {
        va_list argpointer;
        va_start(argpointer, fmt);

        FGUI_VSNPRINTF(fgui_printbuffer, fgui_printbufferlength, fmt, argpointer);

        va_end(argpointer);

        switch(alignment) {
            case fgui_left:
                x=0;
                break;
            case fgui_center:
                x = fgui_strlen(fgui_printbuffer);
                if(x>FGUI_SCR_W) {
                    x=0-((x-FGUI_SCR_W)/2);
                }
                else {
                    x = (FGUI_SCR_W/2) - (x/2);
                }
                break;
            case fgui_right:
                x = fgui_strlen(fgui_printbuffer);
                if(x>FGUI_SCR_W) {
                    x=0-(x-FGUI_SCR_W);
                }
                else {
                    x = FGUI_SCR_W - x;
                }
                break;
            default:
                return 0;
        }
        y=line*fgui_charheight();
        
        if(invert) {
            fgui.fgcolor = ~fgui.fgcolor;
        }
        fgui_clearregion(x, y, FGUI_SCR_W, fgui_charheight());
        fgui_text(x, y, fgui_printbuffer);
        if(invert) {
            fgui.fgcolor = ~fgui.fgcolor;
        }

        return 1;
    }
    return 0;
}

int fgui_lines()
/*
  No. of text-lines that'll fit on the display, using the active font
*/
{
    return FGUI_SCR_H/fgui_charheight();
}

char fgui_progressbar(int x, int y, int w, int h, unsigned char progress, char showText)
/*
  Draw a horizontal progress bar at given x/y location of w*h pixels. 
  Returns 1 on success, false otherwise
*/
{
    int i;

    if(h > 4 && w > 4) {
        /* box */
        fgui_lineh(x, y, w);
        fgui_linev(x, y, h);
        fgui_lineh(x, y + h - 1, w);
        fgui_linev(x + w - 1, y, h);

        /* bar */
        if(progress > 100) {
            progress = 100;
        }
        fgui_clearregion(x+2, y+2, w-4, h-4);
        for(i=0;i<h-4;i++) {
            fgui_lineh(x+2, y+2+i, progress * ((w-4)/100.0));
        }

        /* text */
        if(showText && h-4 > fgui_charheight() && w-4 > fgui_charwidth() * 3) {
            fgui_print(x + (w/2) - fgui_charwidth(), y + (h/2)  - (fgui_charheight()/2), "%d%%", progress);
        }

        return 1;
    }
    return 0;
}

char fgui_scrollbar(int x, int y, int h, unsigned char items, unsigned char location)
/*
  Draw a vertical scroll bar bar at given x/y. 
  Returns 1 on success, false otherwise
*/
{
    double indicatorlength;

    if(h > 6) {
        /* Top terminator (/\) */
        fgui_pixel(x+1, y, FGUI_BLACK);
        fgui_lineh(x, y+1, 3);

        /* Vertical line */
        fgui_linev(x+1, y+3, h-6);

        /* Bottom terminator (\/) */
        fgui_lineh(x, y+h-1, 3);
        fgui_pixel(x+1, y+h, FGUI_BLACK);

        /* Location indicator */
        if(location > items) {
            location = items;
        }
        indicatorlength = (h-6)/items;
        fgui_linev(x,     y + 3 + (location * indicatorlength), indicatorlength);
        fgui_linev(x + 2, y + 3 + (location * indicatorlength), indicatorlength);

        return 1;
    }
    return 0;
}
