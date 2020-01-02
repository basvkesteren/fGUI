/*
   FemtoGUI

    bmpheaders.c

    Copyright (c) 2007,2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#include "bmpheaders.h"

int bmp_readheader(FILE *fptr, bmp_fileheader_t *header, bmp_infoheader_t *infoheader)
{
    /* Read and check the header */
    if (fread(header, sizeof(bmp_fileheader_t), 1, fptr) != 1) {
        printf("Failed to read BMP header\n");
        return 0;
    }
    if(header->type != ('B' | ('M'<<8))) {
        printf("Unexpected header type. Sure this is a bitmap file?\n");
    }
    printf("File size is %i bytes, offset to image data %i bytes\n", header->size, header->offset);

    /* Read and check the information header */
    if (fread(infoheader, sizeof(bmp_infoheader_t), 1, fptr) != 1) {
        printf("Failed to read BMP info header\n");
        return 0;
    }
    /* Size is used to identify the format;
       we expect one of BITMAPINFOHEADER (40 bytes), BITMAPV2INFOHEADER (52 bytes), BITMAPV3INFOHEADER (56 bytes),
       BITMAPV4HEADER (108 bytes) or BITMAPV5HEADER (124 bytes) */
    if(infoheader->size != 40 && infoheader->size != 52 && infoheader->size != 56 && infoheader->size != 108 && infoheader->size != 124) {
        printf("Unexpected infoheader size (%d), bitmap file is not using the 'BITMAPINFOHEADER' format\n", infoheader->size);
        return 0;
    }
    printf("Image size is %i x %i, %i bits/pixel\n", infoheader->width, infoheader->height, infoheader->bits);
    if(infoheader->bits != 1) {
        printf("Only 1 bpp bitmap files are supported\n");
        return 0;
    }
    fprintf(stderr,"Compression type is %i\n", infoheader->compression);
    if(infoheader->compression != 0) {
        printf("Only uncompressed bitmap files are supported\n");
        return 0;
    }

    /* Seek to the start of the image data */
    fseek(fptr, header->offset, SEEK_SET);

    return 1;
}