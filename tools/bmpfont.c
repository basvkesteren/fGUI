/*
    FemtoGUI

    bmpfont.c:
        Simple tool for font generation

    copyright:
              Copyright (c) 2007 Bastiaan van Kesteren <bastiaanvankesteren@gmail.com>

              This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
              This program is free software; you can redistribute it and/or modify it under the terms
              of the GNU General Public License as published by the Free Software Foundation; either
              version 2 of the License, or (at your option) any later version.

    remarks:
            -This tool is written for i386-linux. It *might* work on other platforms,
             but it's never designed to do so, so that would be plain luck.. ;-)
            -When using the -r flag, only readable chars are expected, which are the following:
              [ !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~]
             Excluding the [ at the beginning and the ] at the end.

*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#include "asciitable.h"
#include "bmpheaders.h"

const char outputheader[] ="\
/*\n\
 * This 1bpp font was generated with bmpfont,\n\
 * Copyright (c) 2007 Bastiaan van Kesteren <bas@edeation.nl>\n\
 */\n";

#define LINEBUFFER_SIZE 100

unsigned char charbitlocation;
unsigned int charbytelocation, charbytelocation_start;
char linebuffer[LINEBUFFER_SIZE];

unsigned int bitstobytes(unsigned int bits);
void char_start(unsigned char *chardata, int asciichar, FILE *fptr);
void char_addbit(unsigned char *chardata,unsigned char bit);
void char_finishrow(unsigned char *chardata,FILE *fptr);

void print_usage()
{
    printf("Usage: bmpfont -i<filename> -o<filename> -w<charwidth>\n");
    printf("Required parameters are:\n");
    printf(" -i<filename>    Input file, 1bpp uncompressed BMP file\n");
    printf(" -o<filename>    File used to write the output to\n");
    printf(" -w<charwidth>   Specifies width of one character\n");
    printf("The following parameters are optional:\n");
    printf(" -r              Only expect the readable ASCII characters in the\n");
    printf("                 input file (char 0 up to and including 30 and char\n");
    printf("                 127 are ignored) (optional)\n");
    printf(" -s<pixels>      Specify the spacing in pixels between each character\n");
    printf("                 in the input file (optional)\n");
}

int main(int argc,char **argv)
{
    int i, pixelstart, pixelstop;
    unsigned char *pixeldata, *chardata;
    int charsdone;

    char *inputfile,*outputfile;
    unsigned int charspacing_pixels, charwidth_pixels;
    unsigned int charheigth_pixels, fontwidth_pixels;
    unsigned int imagewidth_bytes;
    unsigned int currentchar_startbyte, currentchar_lastbyte;

    bmp_fileheader_t header;
    bmp_infoheader_t infoheader;
    FILE *fptr;

    /* Parse arguments */
    inputfile = NULL;
    outputfile = NULL;
    charspacing_pixels = 0;
    charwidth_pixels = 0;
    while((i = getopt(argc, argv, "i:o:w:s:")) != -1) {
        switch(i) {
            case 'i':
                inputfile=optarg;
                break;
            case 'o':
                outputfile=optarg;
                break;
            case 'w':
                charwidth_pixels = atoi(optarg);
                break;
            case 's':
                charspacing_pixels = atoi(optarg);
                break;
        }
    }

    /* Check arguments */
    if(inputfile == NULL || outputfile == NULL || charwidth_pixels == 0) {
        print_usage();
        return 0;
    }

     /* Open file */
    if ((fptr = fopen(inputfile,"r")) == NULL) {
        printf("Unable to open BMP file \"%s\"\n",argv[1]);
        return 0;
    }

    /* Read and check fileheaders */
    if(bmp_readheader(fptr, &header, &infoheader) == 0) {
        return 0;
    }

    /* Make sure the font will fit in the inputfile with the given settings */
    charheigth_pixels = infoheader.height;
    fontwidth_pixels = ((ASCII_READABLE_CHARS - 2) * charspacing_pixels) + (ASCII_READABLE_CHARS * charwidth_pixels);
    printf("Character width %i, height %i, spacing %i\n", charwidth_pixels, charheigth_pixels, charspacing_pixels);
    printf("Expect %d characters in bitmap, should be %d pixels width\n", ASCII_READABLE_CHARS, fontwidth_pixels);
    if(infoheader.width < fontwidth_pixels) {
        printf("Imagefile is too small, font won't fit\n");
        return 0;
    }
    else if(infoheader.width > fontwidth_pixels) {
        printf("Warning, imagefile is wider than needed\n");
    }

    /* Read the image */
    imagewidth_bytes = (infoheader.width/8);
    if(infoheader.width%8) {
        imagewidth_bytes++;
    }
    while(imagewidth_bytes%4) {
        imagewidth_bytes++;
    }
    printf("pixeldata is %i bytes width (%i bits padding), total %i bytes\n", imagewidth_bytes, (imagewidth_bytes * 8) - infoheader.width, infoheader.height * imagewidth_bytes);
    pixeldata = malloc(infoheader.height * imagewidth_bytes);
    if(pixeldata == NULL) {
        printf("Not enough memory to read pixeldata\n");
        return 0;
    }
    if(fread(pixeldata,sizeof(unsigned char), infoheader.height * imagewidth_bytes, fptr) != infoheader.height * imagewidth_bytes) {
        printf("Image read failed(pixeldata)\n");
        return 0;
    }
    fclose(fptr);

    /* Open output file */
    if ((fptr = fopen(outputfile,"w")) == NULL) {
        printf("Unable to open outputfile \"%s\"\n", outputfile);
        return 0;
    }

    /* Figure out how much memory is needed for one char, and allocate it */
    chardata = malloc(bitstobytes(charwidth_pixels) * charheigth_pixels);
    if(chardata == NULL) {
        printf("Not enough memory to store a single character\n");
        return 0;
    }

    /* Start output */
    fwrite(outputheader, sizeof(unsigned char), strlen(outputheader), fptr);
    sprintf(linebuffer,"#ifndef FONT_H\n#define FONT_H\n\n");
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
    sprintf(linebuffer,"static const unsigned char font[%i + 2] = {\n", ASCII_READABLE_CHARS * (charheigth_pixels * bitstobytes(charwidth_pixels)));
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
    sprintf(linebuffer,"    %i, /* Character width in pixels */\n", charwidth_pixels);
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
    sprintf(linebuffer,"    %i, /* Character height in pixels */\n", charheigth_pixels);
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);

    /* Parse pixeldata, one character per loop */
    charsdone=0;
    for(int asciichar=ASCII_READABLE_START;asciichar<ASCII_READABLE_END;asciichar++) {
        printf("%c:start at pixel %i, stop at pixel %i\n", asciichar,
                                                           (charsdone * charwidth_pixels) + ((charsdone-1) * charspacing_pixels),
                                                           ((charsdone+1) * charwidth_pixels) + ((charsdone) * charspacing_pixels));

        charbytelocation_start=0;
        charbitlocation=7;
        charbytelocation=0;

        sprintf(linebuffer,"    /* char %i, '%c' */\n", asciichar, asciichar);
        fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);

        for(int height=infoheader.height-1;height>=0;height--) {
            /* Startbyte of current character */
            currentchar_startbyte = ((charsdone * charwidth_pixels) + ((charsdone-1) * charspacing_pixels))>>3;
            /* Last byte of current character */
            currentchar_lastbyte = bitstobytes((charsdone * charspacing_pixels) + ((charsdone+1) * charwidth_pixels));

            pixelstop = 0;
            /* Pixel to start reading; characters are not stored on byte-boundaries */
            pixelstart = 7 - (((charsdone * charwidth_pixels) + ((charsdone-1) * charspacing_pixels)) - (currentchar_startbyte*8));

            for(int width=currentchar_startbyte;width<currentchar_lastbyte;width++) {
                if(width+1 == currentchar_lastbyte) {
                    /* Last byte, make sure we don't read too much */
                    pixelstop = ((width*8)+8) - (((charsdone+1) * charwidth_pixels) + (charsdone * charspacing_pixels));
                }
                for(int pixel=pixelstart;pixel>=pixelstop;pixel--) {
                    char_addbit(chardata, ~(pixeldata[width + (height*imagewidth_bytes)]) & (1<<pixel));
                }
                pixelstart=7;
            }
            char_finishrow(chardata, fptr);
        }
        charsdone++;
    }

    sprintf(linebuffer,"};\n\n#endif\n");
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);

    fclose(fptr);
    free(pixeldata);

    return 1;
}

unsigned int bitstobytes(unsigned int bits)
{
    unsigned int bytes;

    bytes = bits>>3;
    if(bits%8) {
        bytes++;
    }
    return bytes;
}

void char_addbit(unsigned char *chardata, unsigned char bit)
{
    if(bit) {
        chardata[charbytelocation] |= (1<<charbitlocation);
    }
    else {
        chardata[charbytelocation] &= ~(1<<charbitlocation);
    }

    if(charbitlocation==0) {
        charbytelocation++;
        charbitlocation=7;
    }
    else {
        charbitlocation--;
    }
}

void char_finishrow(unsigned char *chardata, FILE *fptr)
{
    int byte,bit,location;

    printf("charbitlocation %d\n", charbitlocation);

    /* Add padding zero's */
    if(charbitlocation!=7) {
        while(charbitlocation > 0) {
            chardata[charbytelocation] &= ~(1<<charbitlocation);
            charbitlocation--;
        }
        chardata[charbytelocation] &= ~(1<<charbitlocation);
        charbitlocation=7;
        charbytelocation++;
    }

    /* Write bytes to file */
    location = sprintf(linebuffer, "    ");
    for(byte=charbytelocation_start;byte<charbytelocation;byte++) {
        location += sprintf(&linebuffer[location],"0x%02x,",chardata[byte]);
    }
    fwrite(linebuffer,sizeof(unsigned char),strlen(linebuffer),fptr);

    /* And add a readable comment after them */
    location = sprintf(linebuffer,"    /*");
    while(charbytelocation_start < charbytelocation) {
        for(bit=0x80;bit>0;bit>>=1) {
            if(chardata[charbytelocation_start]&bit) {
                linebuffer[location]='X';
            }
            else {
                linebuffer[location]=' ';
            }
            location++;
            if(location == LINEBUFFER_SIZE) {
                linebuffer[location]='\0';
                fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
                location=0;
            }
        }
        charbytelocation_start++;
    }
    sprintf(&linebuffer[location],"*/\n");
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
}
