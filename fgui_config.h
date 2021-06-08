/*
    FemtoGUI

    fgui_config.h

    Copyright (c) 2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#ifndef _FGUI_CONFIG_H_
#define _FGUI_CONFIG_H_

/* Include hardware driver */
#ifdef FGUI_DISPLAY_WS2812
#include "hw/ks0108.h"
#elif defined FGUI_DISPLAY_SSD1322
#include "hw/ssd1322.h"
#elif defined FGUI_DISPLAY_UART
#include "hw/uart_display.h"
#elif defined FGUI_DISPLAY_WS2812
#include "hw/ws2812.h"
#endif

typedef signed short fgui_location;

/* Width of the framebuffer in bytes */
#define FGUI_SCR_BYTEW      ((FGUI_SCR_W + 8 - 1) / 8)

/* Size of the framebuffer in bytes */
#define FGUI_FBSIZE         (FGUI_SCR_BYTEW * FGUI_SCR_H)

#endif /* _FGUI_CONFIG_H_ */