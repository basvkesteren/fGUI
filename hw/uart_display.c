/*
    FemtoGUI

    screen_uart.h:
        screen driver with the UART as output (for testing purposes)

    copyright:
              Copyright (c) 2006-2007 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:
            -This driver is usefull for debugging, for
             real world use it probably doesn't make much sense..

*/
#include "uart_display.h"
#include <stdio.h>
#include "fgui_config.h"

char screen_uart_init()
{
    return 1;
}

char screen_uart_update(const unsigned char *framebuffer)
{
    char x,y;
    signed char i;

    for(y=0;y<FGUI_SCR_H;y++) {
        printf("|");
        for(x=0;x<FGUI_SCR_BYTEW;x++) {
            for(i=7;i>=0;i--) {
                if(((x*8) + i) == FGUI_SCR_W) {
                    break;
                }
                if(framebuffer[(y * FGUI_SCR_BYTEW) + x] & (1<<i) ) {
                    printf("X");
                }
                else {
                    printf(" ");
                }
            }
        }
        printf("|\n\r");
    }
    printf("\n\r");

    return 1;
}
