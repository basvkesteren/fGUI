/*
    FemtoGUI

    ssd1322.h

    Copyright (c) 2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#ifndef _SSD1322_H_
#define _SSD1322_H_

#define FGUI_SCR_W  256
#define FGUI_SCR_H  64

#define screen_init()       ssd1322_init()
#define screen_update(fb)   ssd1322_update(fb)

char ssd1322_init(void);
char ssd1322_update(const unsigned char *framebuffer);

#endif /* _SSD1322_H_ */
