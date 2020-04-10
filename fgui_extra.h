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

/* And a strlen function */
#include <string.h>

typedef enum {
    fgui_left,
    fgui_center,
    fgui_right
} fgui_linealignment_t;

void fgui_setprintbuffer(char* buffer, int bufferlength);
char fgui_print(int x, int y, char *fmt, ...);
char fgui_printline(int y, fgui_linealignment_t alignment, char invert, char *fmt, ...);
int fgui_printblock(int y, char *text, unsigned int start, char showscrollbar);
int fgui_blocklength(int y, char showscrollbar);
int fgui_linetoy(unsigned int line);
int fgui_lines(void);

char fgui_progressbar(int x, int y, int w, int h, unsigned char progress, char showText);
char fgui_scrollbar(int x, int y, int h, unsigned int items, unsigned int location);
char fgui_busyindicator(int x, int y, int w, int h, int tick);

#endif /* FGUI_EXTRA_H */