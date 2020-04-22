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

char fgui_printline(int y, fgui_linealignment_t alignment, char invert, char *fmt, ...)
/*
  Print a line of text at x/y location 0/y. This'll clear the whole line before printing the text!
  Returns 1 on success, false otherwise
*/
{
    int x;

    if(fgui_printbuffer && fgui_printbufferlength) {
        /* Get text-string */
        va_list argpointer;
        va_start(argpointer, fmt);

        FGUI_VSNPRINTF(fgui_printbuffer, fgui_printbufferlength, fmt, argpointer);

        va_end(argpointer);

        /* Determine x location */
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

        /* Print text on screen */
        if(invert) {
            fgui.fgcolor = ~fgui.fgcolor;
        }
        fgui_clearregion(0, y, FGUI_SCR_W, fgui_charheight());
        fgui_text(x, y, fgui_printbuffer);
        if(invert) {
            fgui.fgcolor = ~fgui.fgcolor;
        }

        return 1;
    }
    return 0;
}

int fgui_printblock(int y, char *text, unsigned int start, char showscrollbar)
/*
  Print a block of text (text[start]) starting at x/y location 0/y.
  When 'showscrollbar' > 0 a scrollbar is drawn at the right side of the screen.
  Index of the next char to print is returned.
*/
{
    unsigned int i,x,w;

    if(showscrollbar) {
        w = FGUI_SCR_W - 3;

        /* 'items' argument is round up; for x/y, rounding up is done using (x + y - 1) / y */
        fgui_scrollbar(w, y, FGUI_SCR_H-y, (strlen(text) + fgui_blocklength(y, showscrollbar) - 1) / fgui_blocklength(y, showscrollbar),
                                           start / fgui_blocklength(y, showscrollbar));
    }
    else {
        w = FGUI_SCR_W;
    }

    x=0;
    i = start;
    while(text[i]) {
        fgui_char(x,y,text[i]);
        x+=fgui_charwidth();
        if(x+fgui_charwidth() > w) {
            y+=fgui_charheight();
            if(y+fgui_charheight() > FGUI_SCR_H) {
                i++;
                break;
            }
            x=0;
        }
        i++;
    }
    return i;
}

int fgui_blocklength(int y, char showscrollbar)
/*
  Returns no. of chars that'll fit in one block
*/
{
    if(showscrollbar) {
        return ((FGUI_SCR_H-y)/fgui_charheight()) * ((FGUI_SCR_W - 3) / fgui_charwidth());
    }
    else {
        return ((FGUI_SCR_H-y)/fgui_charheight()) * (FGUI_SCR_W / fgui_charwidth());
    }
}

int fgui_linetoy(unsigned int line)
/*
  Convert given y location to a line location, using the active font
*/
{
    return line*fgui_charheight();
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
        /* Box */
        fgui_lineh(x, y, w);
        fgui_linev(x, y, h);
        fgui_lineh(x, y + h - 1, w);
        fgui_linev(x + w - 1, y, h);

        /* Bar */
        if(progress > 100) {
            progress = 100;
        }
        fgui_clearregion(x+2, y+2, w-4, h-4);
        for(i=0;i<h-4;i++) {
            fgui_lineh(x+2, y+2+i, progress * ((w-4)/100.0));
        }

        /* Text, if there's enough room */
        if(showText && h-4 > fgui_charheight() && w-4 > fgui_charwidth() * 3) {
            fgui_print(x + (w/2) - fgui_charwidth(), y + (h/2)  - (fgui_charheight()/2), "%d%%", progress);
        }

        return 1;
    }
    return 0;
}

char fgui_scrollbar(int x, int y, int h, unsigned int items, unsigned int location)
/*
  Draw a vertical scroll bar bar at given x/y location with given height and a width of 3 pixels.
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
        fgui_lineh(x, y+h-2, 3);
        fgui_pixel(x+1, y+h-1, FGUI_BLACK);

        /* Location indicator */
        if(location >= items-1) {
            location = items-1;
        }

        /* 'indicatorlength' is round up; for x/y, rounding up is done using (x + y - 1) / y */
        indicatorlength = ((h-6.0) + items - 1)/items;
        fgui_linev(x,     y + 3 + (location * (h-6.0)/items), indicatorlength);
        fgui_linev(x + 2, y + 3 + (location * (h-6.0)/items), indicatorlength);

        return 1;
    }
    return 0;
}

char fgui_busyindicator(int x, int y, int w, int h, int tick)
/*
  Draw busy indicator at given x/y location of w*h pixels.
  Returns > 0 on success, 0 otherwise. On successive calls, use the return-value of the previous call for the 'tick' argument
*/
{
    int i;
    int dotspacing = ((w+3-1)/3.0);

    if(tick < 1) {
        tick = 1;
    }
    else if(tick > 3) {
        tick = 2;
    }

    if(h > 2 && w > 5) {
        for(i=0;i<3;i++) {
            if(i == tick-1) {
                /* Filled dot */
                fgui_fillregion(x + (i*dotspacing), y, w/3, h);
            }
            else {
                /* Empty dot */
                fgui_clearregion(x + (i*dotspacing), y, w/3, h);
                fgui_lineh(x + (i*dotspacing), y, w/3);
                fgui_linev(x + (i*dotspacing), y, h);
                fgui_lineh(x + (i*dotspacing), y + h - 1, w/3);
                fgui_linev(x + (i*dotspacing) + (w/3) - 1, y, h);
            }
        }

        tick++;
        if(tick > 3) {
            tick = 1;
        }
        return tick;
    }
    return 0;
}