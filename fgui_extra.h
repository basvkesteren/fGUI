/*
    FemtoGUI

    fgui_extra.h:
        Extra's

    copyright:
              Copyright (c) 2020 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

*/
#ifndef FGUI_EXTRA_H
#define FGUI_EXTRA_H

/* We need a vsnprintf function */
#include <print.h>
#define FGUI_VSNPRINTF        vsnprint

typedef enum {
    fgui_left,
    fgui_center,
    fgui_right
} fgui_linealignment_t;

void fgui_setprintbuffer(char* buffer, int bufferlength);
char fgui_print(int x, int y, char *fmt, ...);
char fgui_printline(int line, fgui_linealignment_t alignment, char invert, char *fmt, ...);
int fgui_lines(void);

char fgui_progressbar(int x, int y, int w, int h, unsigned char progress, char showText);
char fgui_scrollbar(int x, int y, int h, unsigned char items, unsigned char location);

#endif /* FGUI_EXTRA_H */