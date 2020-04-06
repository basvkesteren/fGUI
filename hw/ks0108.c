/*
    FemtoGUI

    ks0108.c

    Copyright (c) 2018,2020 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#include "ks0108.h"
#include "../fgui_config.h"

#include <delay.h>
#include <io.h>

/* I/O pins */
#define KS0108_D0   MBED_DIP30
#define KS0108_D1   MBED_DIP29
#define KS0108_D2   MBED_DIP8
#define KS0108_D3   MBED_DIP7
#define KS0108_D4   MBED_DIP6 
#define KS0108_D5   MBED_DIP5 
#define KS0108_D6   MBED_DIP28 
#define KS0108_D7   MBED_DIP27

#define KS0108_CS1  MBED_DIP14  /* Chip select for column 0-63, active low */
#define KS0108_CS2  MBED_DIP13  /* Chip select for column 64-127, active low */
#define KS0108_RST  MBED_DIP12  /* Reset, active low */
#define KS0108_WR   MBED_DIP11  /* Write(0)/Read(1) selection */
#define KS0108_CD   MBED_DIP10  /* Command(1)/Data(0) selection */
#define KS0108_E    MBED_DIP9   /* Enable signal. Read mode: Data can be read while E is high.
                                                  Write mode: Data is read by the display at the falling edge of E */

/* Data read/write macro's. Assumes datapins are connected in order! */
#define DATABUS_SHIFT 4
#define databus_get()               ((LPC_GPIO0->FIOPIN >> DATABUS_SHIFT) & 0xFF)
#define databus_set(data)           do { \
                                        LPC_GPIO0->FIOMASK = ~(0xFF<<DATABUS_SHIFT); \
                                        LPC_GPIO0->FIOPIN = (data&0xFF)<<DATABUS_SHIFT; \
                                        LPC_GPIO0->FIOMASK = 0; \
                                    } while(0)

/* Input/output selection */
#define ctrllines_output()          LPC_GPIO0->FIODIR |= (KS0108_CS1 | KS0108_CS2 | KS0108_RST | KS0108_WR | KS0108_CD | KS0108_E)

#define databus_input()             do {\
LPC_GPIO0->FIODIR &= ~(KS0108_D0 | KS0108_D1 | KS0108_D2 | KS0108_D3 | KS0108_D4 | KS0108_D5 | KS0108_D6 | KS0108_D7);\
LPC_GPIO0->FIOCLR = MBED_DIP18; /* low = b to a */ \
}while(0)

#define databus_output()            do {\
LPC_GPIO0->FIODIR |= (KS0108_D0 | KS0108_D1 | KS0108_D2 | KS0108_D3 | KS0108_D4 | KS0108_D5 | KS0108_D6 | KS0108_D7);\
LPC_GPIO0->FIOSET = MBED_DIP18; /* high = a to b */ \
}while(0)

/* Control line macro's */
#define lcd_reset_high()            LPC_GPIO0->FIOSET = KS0108_RST
#define lcd_reset_low()             LPC_GPIO0->FIOCLR = KS0108_RST
#define lcd_enable_high()           LPC_GPIO0->FIOSET = KS0108_E
#define lcd_enable_low()            LPC_GPIO0->FIOCLR = KS0108_E
#define lcd_wr_high()               LPC_GPIO0->FIOSET = KS0108_WR
#define lcd_wr_low()                LPC_GPIO0->FIOCLR = KS0108_WR
#define lcd_cd_high()               LPC_GPIO0->FIOSET = KS0108_CD
#define lcd_cd_low()                LPC_GPIO0->FIOCLR = KS0108_CD

#define enable_controller(ctrl)     do {\
                                        if(ctrl==0) { \
                                            LPC_GPIO0->FIOCLR = KS0108_CS1; \
                                        } \
                                        else { \
                                            LPC_GPIO0->FIOCLR = KS0108_CS2; \
                                        } \
                                    } while(0)

#define disable_controller(ctrl)    do {\
                                        if(ctrl==0) { \
                                            LPC_GPIO0->FIOSET = KS0108_CS1; \
                                        } \
                                        else { \
                                            LPC_GPIO0->FIOSET = KS0108_CS2; \
                                        } \
                                    } while(0)

/* Commands */
#define CMD_ONOFF   0x3E        /* Bit 0 determines wheter the display goes on(1) or off(0) */
#define CMD_SETY    0x40        /* Bits 0 to 5 set the Y address */
#define CMD_SETX    0xB8        /* Bits 0 to 2 set the X address (or page) */
#define CMD_SETZ    0xC0        /* Bits 0 to 5 set the Z address (or line) */

/* Status bits */
#define STATUS_BUSY  0x80
#define STATUS_ONOFF 0x20
#define STATUS_RESET 0x10

/* Local functions */
static unsigned char ks0108_getstatus(const char controller);
static void ks0108_sendcommand(const unsigned char command, const char controller);
static void ks0108_senddata(const unsigned char data, const char controller);

char ks0108_init()
/*
  Start the display and clear it
*/
{
    unsigned short i,j;

    LPC_PINCON->PINSEL0 = 0x50;
    LPC_PINCON->PINSEL1 = 0;

    /* Pin initialisation */
    databus_input();
    ctrllines_output();
    lcd_reset_low();
    lcd_cd_low();
    lcd_wr_low();
    lcd_enable_low();
    disable_controller(0);
    disable_controller(1);

    /* Display reset is active low, so we're in reset right now */
    delay_us(10);
    lcd_reset_high();
    /* Wait until display is ready */
    i=0;
    while(ks0108_getstatus(0) & STATUS_BUSY) {
        i++;
        if(i==0) {
            /* Timeout! */
            return 0;
        }
    }
    /* Turn controllers on */
    ks0108_sendcommand(CMD_ONOFF | 1,0);
    ks0108_sendcommand(CMD_ONOFF | 1,1);

    /* Start with controller 0.
       Set startposition.. */
    ks0108_sendcommand(CMD_SETY | 0,0);
    ks0108_sendcommand(CMD_SETZ | 0,0);
    /* And now send data */
    for(i=0;i<FGUI_SCR_BYTEW/2;i++) {
        ks0108_sendcommand(CMD_SETX | i,0);
        for(j=0;j<FGUI_SCR_H;j++) {
            ks0108_senddata(0x00,0);
        }
    }

    /* Now controller 1.
       Set startposition.. */
    ks0108_sendcommand(CMD_SETY | 0,1);
    ks0108_sendcommand(CMD_SETZ | 0,1);
    /* And now send data */
    for(i=0;i<FGUI_SCR_BYTEW/2;i++) {
        ks0108_sendcommand(CMD_SETX | i,1);
        for(j=0;j<FGUI_SCR_H;j++) {
            ks0108_senddata(0x00,1);
        }
    }

    return 1;
}

static unsigned char ks0108_getstatus(const char controller)
/*
  Reads status from display, returns data read
*/
{
    unsigned char status;

    /* Databus is input */
    databus_input();
    /* Set control lines */
    lcd_cd_low();
    lcd_wr_high();
    /* Enable controller */
    enable_controller(controller);

    /* OK, Enable high; command is executed */
    lcd_enable_high();
    /* Wait for the datalines to settle */
    delay_us(1);
    status = databus_get();
    lcd_enable_low();

    /* Disable controller */
    disable_controller(controller);

    return status;
}

static void ks0108_sendcommand(const unsigned char command, const char controller)
/*
  Send the given command to the given controller
*/
{
    /* Wait until the display is ready */
    while(ks0108_getstatus(controller) & STATUS_BUSY);

    /* Set control lines */
    lcd_cd_low();
    lcd_wr_low();
    /* Databus is output */
    databus_output();
    databus_set(command);
    /* Enable controller */
    enable_controller(controller);

    /* OK, Enable high; command is executed */
    lcd_enable_high();
    /* Give the display some time to read it */
    delay_us(1);
    lcd_enable_low();

    /* Disable controller */
    disable_controller(controller);
}

static void ks0108_senddata(const unsigned char data, const char controller)
/*
  Send the given data to the given controller
*/
{
    /* Wait until the display is ready */
    while(ks0108_getstatus(controller) & STATUS_BUSY);

    /* Set control lines */
    lcd_cd_high();
    lcd_wr_low();
    /* Databus is output */
    databus_output();
    databus_set(data);
    /* Enable controller */
    enable_controller(controller);

    /* OK, Enable high; command is executed */
    lcd_enable_high();
    /* Give the display some time to read it */
    delay_us(1);
    lcd_enable_low();

    /* Disable controller */
    disable_controller(controller);
}

char ks0108_update(const unsigned char *framebuffer)
/*
  Update the screen (that is, dump the framebuffer on it)

  Screen is divided in two parts; left half and right half.
  Data is send per 8 pixels, going from the left to the right.
*/
{
    unsigned char pixrow;
    unsigned char pixcol;
    signed char pixbit;     /* these need to get negative, too! */
    signed char pixchar;
    unsigned char pixelcount;
    unsigned char pixbitbuffer=0;

    /* Start with controller 0.
       Set startposition.. */
    ks0108_sendcommand(CMD_SETY | 0,0);
    ks0108_sendcommand(CMD_SETY | 0,1);
    /* Dump the data. We have 64 rows */
    for(pixrow=0;pixrow<64;pixrow+=8) {
        pixcol=0;
        pixelcount=0;

        /* The display is 128 pixels width; here we do the first half (controller 0) */
        ks0108_sendcommand(CMD_SETX | pixrow>>3,0);
        while(pixelcount<64) {
            /* We move through the byte, selecting one bit each time */
            for(pixbit=7;pixbit>=0;pixbit--) {
                /* each bit is selected from the byte below the last one */
                for(pixchar=7;pixchar>=0;pixchar--) {
                    pixbitbuffer<<=1;
                    if(framebuffer[((pixrow + pixchar) * 16) + pixcol] & (1<<pixbit)) {
                        pixbitbuffer++;
                    }
                }
                /* one byte complete, ready to go */
                ks0108_senddata(pixbitbuffer,0);
                pixelcount++;
            }
            /* Move to next column */
            pixcol++;
        }

        /* And now the second half (controller 1) */
        ks0108_sendcommand(CMD_SETX | pixrow>>3,1);
        while(pixelcount<128) {
            /* We move through the byte, selecting one bit each time */
            for(pixbit=7;pixbit>=0;pixbit--) {
                /* each bit is selected from the byte below the last one */
                for(pixchar=7;pixchar>=0;pixchar--) {
                    pixbitbuffer<<=1;
                    if(framebuffer[((pixrow + pixchar) * 16) + pixcol] & (1<<pixbit)) {
                        pixbitbuffer++;
                    }
                }
                /* one byte complete, ready to go */
                ks0108_senddata(pixbitbuffer,1);
                pixelcount++;
            }
            /* Move to next column */
            pixcol++;
        }
    }

    return 1;
}