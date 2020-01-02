/*
    FemtoGUI

    bmpsprite.c

    Copyright (c) 2007, 2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
/*
    -This tool is written for i386-linux. It *might* work on other platforms,
     but it's never designed to do so, so that would be plain luck.. ;-)

*/
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"

#include "bmpheaders.h"

const char outputheader[] ="\
/*\n\
 * This 1bpp sprite was generated with bmpsprite,\n\
 * Copyright (c) 2007 Bastiaan van Kesteren <bas@edeation.nl>\n\
 */\n";

#define LINEBUFFER_SIZE 65535

unsigned char spritebitlocation;
unsigned int spritebytelocation;
char linebuffer[LINEBUFFER_SIZE];

void sprite_addbit(unsigned char *spritedata, unsigned char bit);
void sprite_finishrow(unsigned char *spritedata,FILE *fptr);

void print_usage()
{
    printf("Usage: bmpsprite -i<filename> -o<filename>\n");
    printf("Required parameters are:\n");
    printf(" -i<filename>    Input file, 1bpp uncompressed BMP file\n");
    printf(" -o<filename>    File used to write the output to\n");
}

int main(int argc,char **argv)
{
    int i;
    int pixelstop;
    unsigned char *pixeldata, *spritedata;

    char *inputfile, *outputfile;

    bmp_fileheader_t header;
    bmp_infoheader_t infoheader;
    FILE *fptr;

	unsigned int imagewidth_bytes;

    /* Parse arguments */
    inputfile = NULL;
    outputfile = NULL;
    while((i=getopt(argc, argv, "i:o:")) != -1) {
        switch (i) {
            case 'i':
                inputfile=optarg;
                break;
            case 'o':
                outputfile=optarg;
                break;
        }
    }

    /* Check arguments */
    if(inputfile == NULL || outputfile == NULL) {
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

    /* Read the image */
    imagewidth_bytes = (infoheader.width/8);
    if(infoheader.width%8) {
        imagewidth_bytes++;
    }
    while(imagewidth_bytes%4) {
        imagewidth_bytes++;
    }
    printf("pixeldata is %i bytes width (%i bits padding), total %i bytes\n", imagewidth_bytes, (imagewidth_bytes * 8) - infoheader.width,infoheader.height * imagewidth_bytes);
	if(imagewidth_bytes*8 > LINEBUFFER_SIZE) {
		printf("Sorry, picture is too big..\n");
		return 0;
	}
    pixeldata = malloc(infoheader.height * imagewidth_bytes);
    if(pixeldata == NULL) {
        printf("Not enough memory to read pixeldata\n");
        return 0;
    }
    if(fread(pixeldata,sizeof(unsigned char),infoheader.height * imagewidth_bytes,fptr) != infoheader.height * imagewidth_bytes) {
        printf("Image read failed(pixeldata)\n");
        return 0;
    }
    fclose(fptr);

    /* Open output file */
    if ((fptr = fopen(outputfile,"w")) == NULL) {
        printf("Unable to open outputfile \"%s\"\n",outputfile);
        return 0;
    }

    /* Figure out how much memory is needed for the sprite, and allocate it */
    spritedata = malloc(infoheader.height * imagewidth_bytes);
    if(spritedata == NULL) {
        printf("Not enough memory to store a the sprite\n");
        return 0;
    }

    /* Start output */
	fwrite(outputheader, sizeof(unsigned char), strlen(outputheader), fptr);
    sprintf(linebuffer,"#ifndef SPRITE_H\n#define SPRITE_H\n\n");
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
    sprintf(linebuffer,"static const unsigned char sprite[%i+4] = {\n", infoheader.height * imagewidth_bytes);
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
    sprintf(linebuffer,"    %i,%i, /* Sprite width in pixels (MSB, LSB) */\n", infoheader.width>>8, infoheader.width&0xFF);
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
    sprintf(linebuffer,"    %i,%i, /* Sprite height in pixels (MSB, LSB) */\n", infoheader.height>>8, infoheader.height&0xFF);
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);

    /* Parse pixeldata */
    spritebitlocation=7;
    spritebytelocation=0;
    for(int height=infoheader.height-1;height>=0;height--) {
        pixelstop = 0;
        for(int width=0;width<imagewidth_bytes;width++) {
            if((width*8)+8 > infoheader.width) {
                // Width (converted from bytes to pixels) > bitmap width
                pixelstop = 8-(infoheader.width - (width*8));
            }
            for(int pixel=7;pixel>=pixelstop;pixel--) {
                sprite_addbit(spritedata, ~(pixeldata[(height*imagewidth_bytes)+width]) & (1<<pixel));
            }
        }
        sprite_finishrow(spritedata, fptr);
    }

    sprintf(linebuffer,"};\n\n#endif\n");
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);

    fclose(fptr);
    free(pixeldata);

    return 1;
}

void sprite_addbit(unsigned char *spritedata, unsigned char bit)
{
    if(bit) {
        spritedata[spritebytelocation] |= (1<<spritebitlocation);
    }
    else {
        spritedata[spritebytelocation] &= ~(1<<spritebitlocation);
    }
    if(spritebitlocation==0) {
        spritebytelocation++;
        spritebitlocation=7;
        return;
    }
    spritebitlocation--;
}

void sprite_finishrow(unsigned char *spritedata, FILE *fptr)
{
    int byte,bit,location;

    /* Add padding zero's */
    if(spritebitlocation!=7) {
        while(spritebitlocation>0) {
            spritedata[spritebytelocation] &= ~(1<<spritebitlocation);
            spritebitlocation--;
        }
        spritebitlocation=7;
        spritebytelocation++;
    }

    /* Write bytes to file */
    location = sprintf(linebuffer, "    ");
    for(byte=0;byte<spritebytelocation;byte++) {
        location += sprintf(&linebuffer[location],"0x%02x,",spritedata[byte]);
    }
    fwrite(linebuffer,sizeof(unsigned char),strlen(linebuffer),fptr);

    /* And add a readable comment after them */
    location = sprintf(linebuffer,"    /*");
    byte=0;
    while(byte < spritebytelocation) {
        for(bit=0x80;bit>0;bit>>=1) {
            if(spritedata[byte]&bit) {
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
        byte++;
    }
    spritebytelocation=0;

    sprintf(&linebuffer[location],"*/\n");
    fwrite(linebuffer, sizeof(unsigned char), strlen(linebuffer), fptr);
}