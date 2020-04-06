/*
    FemtoGUI

    ks0108.h

    Copyright (c) 2008,2020 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#ifndef _KS0108_H_
#define _KS0108_H_

#define FGUI_SCR_W  128
#define FGUI_SCR_H  64

#define screen_init()       ks0108_init()
#define screen_update(fb)   ks0108_update(fb)

char ks0108_init(void);
char ks0108_update(const unsigned char *framebuffer);

#endif /* _KS0108_H_ */
