/*
    FemtoGUI

    fgui_sprite.c:
        Sprite functions

    copyright:
              Copyright (c) 2006-2008 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:

*/
#include "fgui.h"
#include <string.h>

#define FGUI_CODE
#include "fgui_helper.h"

extern fgui_t fgui;

void fgui_sprite(const int x, const int y, const unsigned char *sprite)
/*
  Copy a sprite to the given x/y location.
  'sprite' must point to a valid sprite array. Elements in the array are:
   0,1  sprite width in bits (pixels)
   2,3  sprite height in bits (pixels)
   4-n  The actual pixel data
*/
{
    copypixeldata(x, y, &sprite[4], (sprite[0]<<8) | sprite[1], (sprite[2]<<8) | sprite[3], bitstobytesup((sprite[0]<<8) | sprite[1]), fgui.fgcolor);
}

int fgui_spritewidth(const unsigned char *sprite)
{
    return (sprite[0]<<8) | sprite[1];
}

int fgui_spriteheight(const unsigned char *sprite)
{
    return (sprite[2]<<8) | sprite[3];
}