/*
    FemtoGUI

    ws2812.c

    Copyright (c) 2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#ifndef _WS2812_H_
#define _WS2812_H_

#define FGUI_SCR_W  24
#define FGUI_SCR_H  20

#define screen_init()       ws2812_init()
#define screen_update(fb)   ws2812_update(fb)

char ws2812_init(void);
char ws2812_update(const unsigned char *framebuffer);

void ws2812_setcolors(unsigned char red_on, unsigned char green_on, unsigned char blue_on, unsigned char red_off, unsigned char green_off, unsigned char blue_off);

#endif /* _WS2812_H_ */