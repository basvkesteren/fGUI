/*
    FemtoGUI

    bmpheaders.h

    Copyright (c) 2019 Bastiaan van Kesteren <bas@edeation.nl>
    This program comes with ABSOLUTELY NO WARRANTY; for details see the file LICENSE.
    This program is free software; you can redistribute it and/or modify it under the terms
    of the GNU General Public License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
*/
#ifndef _BMPHEADERS_H_
#define _BMPHEADERS_H_

#include "types.h"
#include "stdio.h"

typedef struct __attribute__ ((packed)) {
   u16_t type;                      /* Magic identifier            */
   u32_t size;                      /* File size in bytes          */
   u32_t reserved;
   u32_t offset;                    /* Offset to image data, bytes */
} bmp_fileheader_t;

typedef struct __attribute__ ((packed)) {
   u32_t size;                      /* Header size in bytes      */
   s32_t width, height;             /* Width and height of image */
   u16_t planes;                    /* Number of colour planes   */
   u16_t bits;                      /* Bits per pixel            */
   u32_t compression;               /* Compression type          */
   u32_t imagesize;                 /* Image size in bytes       */
   s32_t xresolution,yresolution;   /* Pixels per meter          */
   u32_t ncolours;                  /* Number of colours         */
   u32_t importantcolours;          /* Important colours         */
} bmp_infoheader_t;

int bmp_readheader(FILE *fptr, bmp_fileheader_t *header, bmp_infoheader_t *infoheader);

#endif /* _BMPHEADERS_H_ */