#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"

typedef struct{
  unsigned char *data;
  unsigned int width;
  unsigned int height;
  unsigned int ch;
} BITMAPDATA_t;

int pngFileReadDecode(BITMAPDATA_t *, const char*);
int pngFileEncodeWrite(BITMAPDATA_t *, const char*);
int freeBitmapData(BITMAPDATA_t *);

#define SIGNATURE_NUM 8

