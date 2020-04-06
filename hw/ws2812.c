/*
    FemtoGUI

    ws2812.c

    Copyright (c) 2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#include "ws2812.h"
#include "delay.h"
#include "io.h"
#include "../fgui_config.h"

static unsigned char red, green, blue;
static unsigned char red_off, green_off, blue_off;

char ws2812_init()
{
	ws2812_setcolors(16, 0, 0, 0, 0, 1);

    return 1;
}

void ws2812_setcolors(unsigned char fg_red, unsigned char fg_green, unsigned char fg_blue, unsigned char bg_red, unsigned char bg_green, unsigned char bg_blue)
{
	red = fg_red;
	green = fg_green;
	blue = fg_blue;

	red_off = bg_red;
	green_off = bg_green;
	blue_off = bg_blue;
}

#ifdef __SDCC
#define ws2812_bitdelay() \
do { \
	__asm__ ("nop"); \
	__asm__ ("nop"); \
	__asm__ ("nop"); \
	__asm__ ("nop"); \
} while(0)
#else
#define ws2812_bitdelay() \
do { \
	asm ("nop"); \
	asm ("nop"); \
	asm ("nop"); \
	asm ("nop"); \
} while(0)
#endif

#define ws2812_bit_one() \
do { \
	PORTAbits.RA1 = 1; \
	ws2812_bitdelay(); \
	ws2812_bitdelay(); \
	PORTAbits.RA1 = 0; \
	ws2812_bitdelay(); \
} while(0)

#define ws2812_bit_zero() \
do { \
	PORTAbits.RA1 = 1; \
	ws2812_bitdelay(); \
	PORTAbits.RA1 = 0; \
	ws2812_bitdelay(); \
	ws2812_bitdelay(); \
} while(0)

void ws2812_byte(unsigned char value)
{
	if(value & (1<<7)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
	if(value & (1<<6)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
	if(value & (1<<5)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
	if(value & (1<<4)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
	if(value & (1<<3)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
	if(value & (1<<2)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
	if(value & (1<<1)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
	if(value & (1<<0)) { ws2812_bit_one(); } else { ws2812_bit_zero(); } \
}

void ws2812_led(volatile unsigned char g, volatile unsigned char r, volatile unsigned char b)
{
	ws2812_byte(g);
	ws2812_byte(r);
	ws2812_byte(b);
}

char ws2812_update(const unsigned char *framebuffer)
{
	unsigned short pixelbyte = FGUI_FBSIZE-FGUI_SCR_BYTEW;
	unsigned char line, pixelbyte_lineoffset, pixel;
	unsigned char line_is_odd;

	#if FGUI_SCR_H%1 > 0
	line_is_odd = 0xFF;
	#else
	line_is_odd = 0x00;
	#endif

    INTCONbits.GIE = 0;

	for(line=FGUI_SCR_H;line>0;line--) {
		if(line_is_odd) {
			pixelbyte_lineoffset=0;
			do {
				for(pixel=0x80;pixel>0;pixel>>=1) {
					if(framebuffer[pixelbyte + pixelbyte_lineoffset] & pixel) {
						/* Pixel on */
						ws2812_led(green, red, blue);
					}
					else {
						/* Pixel off */
						ws2812_led(green_off, red_off, blue_off);
					}
				}
				pixelbyte_lineoffset++;
			} while (pixelbyte_lineoffset < FGUI_SCR_BYTEW);
		}
		else {
            pixelbyte_lineoffset=FGUI_SCR_BYTEW;
			do {
				pixelbyte_lineoffset--;
				pixel=1;
				do {
					if(framebuffer[pixelbyte + pixelbyte_lineoffset] & pixel) {
						/* Pixel on */
						ws2812_led(green, red, blue);
					}
					else {
						/* Pixel off */
						ws2812_led(green_off, red_off, blue_off);
					}
					pixel<<=1;
				} while(pixel);
			} while(pixelbyte_lineoffset > 0);
		}
		pixelbyte -= FGUI_SCR_BYTEW;
		line_is_odd = ~line_is_odd;
	}

    INTCONbits.GIE = 1;

	return 1;
}