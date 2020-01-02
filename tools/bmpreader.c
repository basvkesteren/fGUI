/*
    FemtoGUI

    bmpreader.c

    Copyright (c) 2007 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
/*
    -This tool is written for i386-linux. It *might* work on other platforms,
      but it's never designed to do so, so that would be plain luck.. ;-)

    -This program doesn't do anything usefull whit regards to fGUI. It just reads
     the given BMP and displays it on the console, which can be used to check the
     BMP reader code, and to see how your picture would look when replacing pixels
     with 'X'-es (would you want that.. )
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bmpheaders.h"

int main(int argc,char **argv)
{
    int i,j,k,bytewidth,pixelstop;
    unsigned char *pixeldata;
    bmp_fileheader_t header;
    bmp_infoheader_t infoheader;
    FILE *fptr;

    /* Check arguments */
    if (argc < 2) {
        printf("Usage: %s filename\n",argv[0]);
        return 0;
    }

    /* Open file */
    if ((fptr = fopen(argv[1],"r")) == NULL) {
        printf("Unable to open BMP file \"%s\"\n",argv[1]);
        return 0;
    }

    /* Read and check fileheaders */
    if(bmp_readheader(fptr, &header, &infoheader) == 0) {
        return 0;
    }

    /* Read the image */
    bytewidth = (infoheader.width/8);
    if(infoheader.width%8) {
        bytewidth++;
    }
    while(bytewidth%4) {
        bytewidth++;
    }
    printf("pixeldata is %i bytes width (%i bits padding), total %i bytes\n", bytewidth, (bytewidth * 8) - infoheader.width, infoheader.height * bytewidth);
    pixeldata = malloc(infoheader.height * bytewidth);
    if(pixeldata == NULL) {
        printf("Not enough memory to read pixeldata\n");
        return 0;
    }
    if(fread(pixeldata, sizeof(unsigned char), infoheader.height * bytewidth, fptr) != infoheader.height * bytewidth) {
        printf("Image read failed\n");
        return 0;
    }
    fclose(fptr);

    /* Parse pixeldata */
    for (j=infoheader.height-1;j>=0;j--) {
        printf("[");
        pixelstop = 0;
        for (i=0;i<bytewidth;i++) {
            if((i*8)+8>infoheader.width) {
                pixelstop = 8-(infoheader.width - (i*8));
            }
            for(k=7;k>=pixelstop;k--) {
                if(pixeldata[(j*bytewidth)+i]&(1<<k)) {
                    printf(" ");
                }
                else {
                    printf("X");
                }
            }
        } /* width (i) */
        printf("]\n");
    } /* height (j) */

    free(pixeldata);

    return 1;
}

