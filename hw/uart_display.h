/*
    FemtoGUI

    uart_display.h:
        screen driver whit the UART as output (for testing purposes)

    copyright:
              Copyright (c) 2006-2007 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

*/
#ifndef _UART_DISPLAY_H_
#define _UART_DISPLAY_H_

#define FGUI_SCR_W  24
#define FGUI_SCR_H  20

#define screen_init()       screen_uart_init()
#define screen_update(fb)   screen_uart_update(fb)

char uart_display_init(void);
char uart_display_update(const unsigned char *framebuffer);

#endif /* _UART_DISPLAY_H_ */
