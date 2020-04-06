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

/* We need an vsnprintf function */
#include <print.h>
#define FGUI_VSNPRINTF        vsnprint

char fgui_progressbar(int x, int y, int w, int h, int progress, char showText);
void fgui_print(char *buffer, int bufferlength, int x, int y, char *fmt, ...);

#endif /* FGUI_EXTRA_H */