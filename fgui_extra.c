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

char fgui_progressbar(int x, int y, int w, int h, int progress, char showText)
{
    int i;
    char text[5];

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
            fgui_print(text, sizeof(text), x + (w/2) - fgui_charwidth(), y + (h/2)  - (fgui_charheight()/2), "%d%%", progress);
        }

        return 1;
    }
    return 0;
}

void fgui_print(char *buffer, int bufferlength, int x, int y, char *fmt, ...)
{
    va_list argpointer;
    va_start(argpointer, fmt);

    FGUI_VSNPRINTF(buffer, bufferlength, fmt, argpointer);
    fgui_text(x, y, buffer);

    va_end(argpointer);
}