/*
    FemtoGUI

    ssd1322.c

    Copyright (c) 2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#include "ssd1322.h"

#include <ssp.h>
#include <delay.h>
#include <hardware/io.h>

#define SSD1322_CMD_COLUMN_ADDRESS      0x15
#define SSD1322_CMD_WRITE_RAM           0x5C
#define SSD1322_CMD_ROW_ADDRESS         0x75
#define SSD1322_CMD_REMAP               0xA0
#define SSD1322_CMD_STARTLINE           0xA1
#define SSD1322_CMD_VERT_SCROLL         0xA2
#define SSD1322_CMD_MODE_OFF            0xA4
#define SSD1322_CMD_MODE_FULLON         0xA5
#define SSD1322_CMD_MODE_NORMAL         0xA6
#define SSD1322_CMD_MODE_INVERSE        0xA7
#define SSD1322_CMD_EXIT_PARTIAL_MODE   0xA9
#define SSD1322_CMD_VDD                 0xAB
#define SSD1322_CMD_DISPLAY_OFF         0xAE
#define SSD1322_CMD_DISPLAY_ON          0xAF
#define SSD1322_CMD_PHASE               0xB1
#define SSD1322_CMD_CLOCK               0xB3
#define SSD1322_CMD_ENHANCEMENT_A       0xB4
#define SSD1322_CMD_GPIO                0xB5
#define SSD1322_CMD_2ND_PRECHARGE       0xB6
#define SSD1322_CMD_GRAYSCALE_TABLE     0xB8
#define SSD1322_CMD_PRECHARGE           0xBB
#define SSD1322_CMD_VCOM                0xBE
#define SSD1322_CMD_CONTRAST_CURRENT    0xC1
#define SSD1322_CMD_CONTRAST_MASTER     0xC7
#define SSD1322_CMD_MUX                 0xCA
#define SSD1322_CMD_ENHANCEMENT_B       0xD1
#define SSD1322_CMD_LOCK                0xFD

#define ssd1322_pin_select()            DISPLAY_CS_ACTIVATE()
#define ssd1322_pin_deselect()          DISPLAY_CS_DEACTIVATE()
#define ssd1322_pin_command()           DISPLAY_COMMAND()
#define ssd1322_pin_data()              DISPLAY_DATA()

static void ssd1322_tx(unsigned char* data, int length)
{
    for(int i=0;i<length;i++) {
        ssp1_put(data[i]);
    }
}

static void ssd1322_cmd(unsigned char command)
{
    ssd1322_pin_command();

    ssd1322_pin_select();
    ssd1322_tx(&command, 1);
    ssd1322_pin_deselect();
}

static void ssd1322_data(unsigned char data)
{
    ssd1322_pin_data();

    ssd1322_pin_select();
    ssd1322_tx(&data, 1);
    ssd1322_pin_deselect();
}

char ssd1322_init()
{
    ssd1322_cmd(SSD1322_CMD_LOCK);
    ssd1322_data(0x12);

    ssd1322_cmd(SSD1322_CMD_DISPLAY_OFF);

    ssd1322_cmd(SSD1322_CMD_CLOCK);
    ssd1322_data(0x91);     // 135 frames/sec

    ssd1322_cmd(SSD1322_CMD_MUX);
    ssd1322_data(0x3F);     // 64-1 ratio

    ssd1322_cmd(SSD1322_CMD_VERT_SCROLL);
    ssd1322_data(0x00);     // No vertical scroll

    ssd1322_cmd(SSD1322_CMD_STARTLINE);
    ssd1322_data(0x00);     // Startline 0

    ssd1322_cmd(SSD1322_CMD_REMAP);
    ssd1322_data(0x14); // Horizontal address increment, enable column and nibble re-map, scan from COM[N-1] - COM0, disable COM split odd even
    ssd1322_data(0x11); // Dual COM mode

    ssd1322_cmd(SSD1322_CMD_GPIO);
    ssd1322_data(0x00);     // Disable GPIO

    ssd1322_cmd(SSD1322_CMD_VDD);
    ssd1322_data(0x01); // Enable internal VDD regulator

    ssd1322_cmd(SSD1322_CMD_ENHANCEMENT_A);
    ssd1322_data(0xA0); // External VSL
    ssd1322_data(0xFD); // Enhanced low grayscale display

    ssd1322_cmd(SSD1322_CMD_CONTRAST_CURRENT);
    ssd1322_data(0xFF); // Set contrast current, full

    ssd1322_cmd(SSD1322_CMD_CONTRAST_MASTER);
    ssd1322_data(0x0F); // Set master contrast, full

    ssd1322_cmd(SSD1322_CMD_GRAYSCALE_TABLE);
    ssd1322_data(0x00); // Gamma Setting for GS1
    ssd1322_data(0x00);
    ssd1322_data(0x00);
    ssd1322_data(0x03);
    ssd1322_data(0x06);
    ssd1322_data(0x10);
    ssd1322_data(0x1D);
    ssd1322_data(0x2A);
    ssd1322_data(0x37);
    ssd1322_data(0x46);
    ssd1322_data(0x58);
    ssd1322_data(0x6A);
    ssd1322_data(0x7F);
    ssd1322_data(0x96);
    ssd1322_data(0xB4); // Gamma Setting for GS15

    ssd1322_cmd(SSD1322_CMD_PHASE);
    ssd1322_data(0xE8); // Phase 1 = 13 DCLK's, phase 2 = 14 DCLK's

    ssd1322_cmd(SSD1322_CMD_ENHANCEMENT_B);
    ssd1322_data(0x82); // Normal
    ssd1322_data(0x20);

    ssd1322_cmd(SSD1322_CMD_PRECHARGE);
    ssd1322_data(0x1F); // Pre-charge voltage = 0.6xVcc

    ssd1322_cmd(SSD1322_CMD_2ND_PRECHARGE);
    ssd1322_data(0x08); // 2nd pre-charge period =8 DCLK's

    ssd1322_cmd(SSD1322_CMD_VCOM);
    ssd1322_data(0x7); // COM deselect voltage = 0.86xVCC

    ssd1322_cmd(SSD1322_CMD_EXIT_PARTIAL_MODE);

    ssd1322_cmd(SSD1322_CMD_MODE_NORMAL);

    delay_ms(1);    //stabilize VDD

    ssd1322_cmd(SSD1322_CMD_DISPLAY_ON);

    delay_ms(200);    //stabilize VDD

    ssd1322_cmd(SSD1322_CMD_COLUMN_ADDRESS);
    ssd1322_data(0x1C); // Start column
    ssd1322_data(0x5B); // Stop column

    ssd1322_cmd(SSD1322_CMD_ROW_ADDRESS);
    ssd1322_data(0x00); // Start row
    ssd1322_data(0x3F); // Stop row

    return 1;
}

char ssd1322_update(const unsigned char *framebuffer)
{
    unsigned char row, column;
    unsigned char pixelbyte, pixel;
    unsigned char grayscalepixels[4];

    ssd1322_cmd(SSD1322_CMD_WRITE_RAM);

    ssd1322_pin_data();
    ssd1322_pin_select();
    for(row=0;row<FGUI_SCR_H;row++) {
        for(column=0;column<FGUI_SCR_BYTEW;column++) {
            pixelbyte = framebuffer[(row * FGUI_SCR_BYTEW) + column];

            for(pixel=0;pixel<4;pixel++) {
                switch((pixelbyte&0xC0)>>6) {
                    case 0: //00
                        grayscalepixels[pixel] = 0x00;
                        break;
                    case 1: //01
                        grayscalepixels[pixel] = 0x0F;
                        break;
                    case 2: //10
                        grayscalepixels[pixel] = 0xF0;
                        break;
                    case 3: //11
                        grayscalepixels[pixel] = 0xFF;
                        break;
                    }
                    pixelbyte<<=2;
                }

            ssd1322_tx(grayscalepixels, 4);
        }
    }
    ssd1322_pin_deselect();

    return 1;
}
