# fGUI

This is fGUI, a (Very) simple and lightweight 1-bit-per-pixel graphics library 
('library' is probably a too big word for this..)

![fGUI](fgui.jpg?raw=true "fGUI")

## Functionality

fGUI can:
-Turn individual pixels on and off
-Draw lines
-Draw text (with an included tool to generate fonts)
-Draw sprites (with an included tool to generate sprites)

It's purpose is to be a super-simple collection of functions to be used in
embedded systems, to drive monochrome displays. It defines a piece of RAM
(the framebuffer) and some functions to draw lines, text and whatnot in
this framebuffer. A 'refresh' function is used to send this framebuffer to
the display. The display-code is (obviously) hardware-dependend. Some
display-code is included as an example.

## Why

I created fGUI back in 2006 because, well, I wanted to use a graphical LCD on
a PICmicro project. Notice how this is from before the 'arduino era' we live in
today, u8glib and the likes didn't exist back then.

## Current status

Works for me (tm)
 
## How to use

Place the fGUI sources in a subfolder of your project, include the Makefile.inc
file in your Makefile (which assumes SRC defines the .c files to compile)

Include fgui.h and any font/sprite header files you've generated in your code,
and define the framebuffer ('unsigned char framebuffer[FGUI_FBSIZE];').

At startup call fgui_init to setup fGUI, call fgui_refresh every time you want
to update the display

```c
    #include <fgui.h>
    #include "font.h"

    ......
    
    unsigned char framebuffer[FGUI_FBSIZE];
    
    ......

    fgui_init(framebuffer);     /* Initialise screen.. */
    fgui_clear();               /* ..clear framebuffer..*/
    fgui_setfont(font);         /* .. and set font */

    fgui_refresh();             /* Update display */
```

## How to get fonts and sprites

In the 'tools' directory you'll find some console tools, all written for GCC
on Linux (might work on other platforms too, haven't tried..).
All these programs accept 1bpp BMP pictures; the program 'bmpfont' can be used
to create fonts, 'bmpsprite' to create sprites. The third program, 'bmpreader',
can be used to test the bmp reader code used by the other two programs.

An example sprite is included, the following command will generate a header-file
from it:

```sh
    ./bmpsprite -i example_sprite.bmp -o example_sprite.h
    File size is 210 bytes, offset to image data 130 bytes
    Image size is 20 x 20, 1 bits/pixel
    Compression type is 0
    pixeldata is 4 bytes width (12 bits padding), total 80 bytes
```

Same goes for font-generation:

```sh
    ./bmpfont -i example_font.bmp -w 7 -o example_font.h
    File size is 1138 bytes, offset to image data 130 bytes
    Image size is 665 x 12, 1 bits/pixel
    Compression type is 0
    Character width 7, height 12, spacing 0
    Expect 95 characters in bitmap, should be 665 pixels width
    pixeldata is 84 bytes width (7 bits padding), total 1008 bytes
    :start at pixel 0, stop at pixel 7
    
    ......

    }:start at pixel 651, stop at pixel 658
    ~:start at pixel 658, stop at pixel 665
```