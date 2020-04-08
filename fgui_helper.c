/*
    FemtoGUI

    fgui_helper.c:
        Internal helper functions

    copyright:
              Copyright (c) 2006-2008 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:

*/
#include "fgui.h"

#define FGUI_CODE
#include "fgui_helper.h"

extern fgui_t fgui;

unsigned int bitstobytesup(const unsigned int bits)
/*
  Give no. of bytes needed for the given no. of bits, rounding up
*/
{
    unsigned int bytes;

    bytes=bits>>3;
    if(bits%8) {
        bytes++;
    }
    return bytes;
}

unsigned int bitstobytesdown(const unsigned int bits)
/*
  Give no. of bytes needed for the given no. of bits, ignoring remainders
*/
{
    return bits>>3;
}

static unsigned char getpixelbyte(const unsigned char *pixeldata, int location, unsigned char fgcolor)
/*
  Return byte from pixeldata[location], inverted if fgcolor != FGUI_BLACK
*/
{
    if(fgcolor) {
        return pixeldata[location];
    }
    else {
        return ~pixeldata[location];
    }
}

void copypixeldata(int x, int y, const unsigned char *pixeldata, int w, int h, unsigned int bytew, unsigned char fgcolor)
/*
  Copy 'pixeldata' with width 'w' and height 'h' (both in pixels/bits) to framebuffer-location x/y, where both x and y can be negative
*/
{
    unsigned int pixeldata_skip_h, pixeldata_skip_w;
    unsigned char bitmask_left, bitmask_right, bitmask_leftovers;
    int i_h, i_w;

    /* Cleanup y/h parameters */
    if(y < 0) {
        /* Negative y means upper rows of pixeldata will be skipped */
        h += y;
        pixeldata_skip_h = 0-y;
        y = 0;
    }
    else {
        pixeldata_skip_h = 0;
    }
    if(y + h > FGUI_SCR_H) {
        /* Make sure we won't write beyond the framebuffer by skipping lower rows of pixeldata */
    	h = FGUI_SCR_H-y;
    }

    /* Cleanup x/w parameters */
    if(x < 0) {
        /* Negative x means left colums of pixeldata will be skipped */
        w += x;
        pixeldata_skip_w = 0-x;
        x = 0;
    }
    else {
        pixeldata_skip_w = 0;
    }
    if(x+w > FGUI_SCR_W) {
        /* Make sure we won't write beyond the framebuffer by skipping right colums of pixeldata */
    	w = FGUI_SCR_W-x;
    }

    if(h <= 0 || w <= 0) {
    	/* Nothing left to copy! */
	    return;
    }

    /* Multiple cases, example with same pixeldata (w=20), x differs */
    if(x == 0 && pixeldata_skip_w%8 > 0) {
        /* Case 1: pixeldata is not aligned with framebuffer, x < 0: (NOTE: at this point x is corrected to 0,
                   w modified and pixeldata_skip_w is set!).
            0         1         2
            [70123456][70123___][________] (w = 13, pixeldata_skip_w = 7)
         */
        if(w>=8) {
             /* There are complete bytes to copy */
            for(i_h=0;i_h<h;i_h++) {
                /* Height/row loop. Per row, copy all complete bytes */
                for(i_w=0;i_w<w>>3;i_w++) {
                    /* Width/column loop. Copy all complete bytes (w=0 to w/8) */
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] = (getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + (pixeldata_skip_w>>3) + i_w, fgcolor) << (pixeldata_skip_w%8)) |
                                                                           (getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + (pixeldata_skip_w>>3) + i_w + 1, fgcolor) >> (8-(pixeldata_skip_w%8)));
                }
            }
        }
        if(w%8) {
            /* There are leftovers to copy */
            i_w = w>>3;
            bitmask_leftovers = 0xFF << (8-(w%8));                      // Bitmask for the leftovers ([70123___])

            if((w%8) + (pixeldata_skip_w%8) > 8) {
                /* Leftovers are spread over two pixeldata-bytes (see initial example of this case) */
                for(i_h=0;i_h<h;i_h++) {
                    /* Height/row loop. Per row, copy the leftover byte (the last column, at w/8) */
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] &= ~bitmask_leftovers;
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] |= ((getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + (pixeldata_skip_w>>3) + i_w, fgcolor) << (pixeldata_skip_w%8)) |
                                                                            (getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + (pixeldata_skip_w>>3) + i_w + 1, fgcolor) >> (8-(pixeldata_skip_w%8)))) & bitmask_leftovers;
                }
            }
            else {
                /* Leftovers are in one pixeldata-byte:
                    0         1         2
                    [12345670][12345670][123_____] (w = 19, pixeldata_skip_w = 1)
                 */
                for(i_h=0;i_h<h;i_h++) {
                    /* Height/row loop. Per row, copy the leftover byte (the last column, at w/8) */
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] &= ~bitmask_leftovers;
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] |= (getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + (pixeldata_skip_w>>3) + i_w, fgcolor) << (pixeldata_skip_w%8)) & bitmask_leftovers;
                }                                                                                    
            }
        }
    }
    else if(x%8 == 0 && pixeldata_skip_w%8 == 0) {
        /* Case 2: pixeldata is aligned with framebuffer, x = 0 (and all subsequent powers of 8, including negatives. Note that at this
                   point a negative x is corrected to 0, with pixeldata_skip_w set):
            0         1         2
            [01234567][01234567][0123____]
         */
        if(w>=8) {
            /* There are complete bytes to copy */
            for(i_h=0;i_h<h;i_h++) {
                /* Height/row loop. Per row, copy all complete bytes */
                for(i_w=0;i_w<w>>3;i_w++) {
                    /* Width/column loop. Copy all complete bytes (w=0 to w/8) */
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] = getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + (pixeldata_skip_w>>3) + i_w, fgcolor);
                }
            }
        }
        if(w%8) {
            /* There are leftovers to copy */
            bitmask_leftovers = 0xFF << (8-(w%8));                      // Bitmask for the leftovers ([0123____])

            i_w = w>>3;
            for(i_h=0;i_h<h;i_h++) {
                /* Height/row loop. Per row, copy the leftover byte (the last column, at w/8) */
                fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] &= ~bitmask_leftovers;
                fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] |= getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + (pixeldata_skip_w>>3) + i_w, fgcolor);
            }
        }
    }
    else if(x%8 > 0) {
        /* Case 3: pixeldata is not aligned with framebuffer, x > 0 (or anything but a power of 8).
                   Since x > 0, we know pixeldata_skip_w == 0:
            0         1         2
            [_0123456][70123456][70123___]
         */
        bitmask_right = 0xFF >> (x%8);                              // Bitmask for the right part of each framebuffer-byte ([_0123456])
        bitmask_left = 0xFF << (8-(x%8));	                        // Bitmask for the left part of each framebuffer-byte ([7_______])

        if(w>=8) {
            /* There are complete bytes to copy */
            for(i_h=0;i_h<h;i_h++) {
                /* Height/row loop. Per row, copy all complete bytes */
                for(i_w=0;i_w<w>>3;i_w++) {
                    /* Width/column loop. Copy all complete bytes (w=0 to w/8) */
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] &= ~bitmask_right;
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] |= getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + i_w, fgcolor) >> (x%8);

                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w + 1] &= ~bitmask_left;
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w + 1] |= getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + i_w, fgcolor) << (8-(x%8));
                }
            }
        }
        if(w%8) {
            /* There are leftovers to copy */
            i_w = w>>3;

            if((w%8)+(x%8) < 8) {
                /* Pixeldata-leftovers go into a single framebuffer byte: (see initial example of this case) */
                bitmask_leftovers = bitmask_right & (0xFF << (8-((w%8)+(x%8))));// Bitmask for the leftovers ([_0123___])

                for(i_h=0;i_h<h;i_h++) {
                    /* Height/row loop. Per row, copy the leftover byte (the last column, at w/8) */
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] &= ~bitmask_leftovers;
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] |= (getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + i_w, fgcolor) >> (x%8)) & bitmask_leftovers;
                }
            }
            else {
                /* Pixeldata-leftovers are spread over two framebuffer bytes:
                    0         1         2
            	    [_______0][12345670][123_____]
                 */
                bitmask_leftovers = bitmask_left << (8-(w%8));              // Bitmask for the leftovers ([123_____])

                for(i_h=0;i_h<h;i_h++) {
                    /* Height/row loop. Per row, copy the leftover byte (the last column, at w/8) */
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] &= ~bitmask_right;
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w] |= getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + i_w, fgcolor) >> (x%8);

                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w + 1] &= ~bitmask_leftovers;
                    fgui.fb[((y + i_h) * FGUI_SCR_BYTEW) + (x>>3) + i_w + 1] |= (getpixelbyte(pixeldata, ((i_h + pixeldata_skip_h) * bytew) + i_w, fgcolor) << (8-(x%8))) & bitmask_leftovers;
                }
            }
        }
    }
}
