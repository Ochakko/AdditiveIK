#include "myPng.h"

int pngFileReadDecode(BITMAPDATA_t *bitmapData, const char* filename){

  FILE *fi;
  int j;
  unsigned int width, height;
  unsigned int readSize;

  png_structp png;
  png_infop info;
  png_bytepp datap;
  png_byte type;
  png_byte signature[8];

  fi = fopen(filename, "rb");
  if(fi == NULL){
    printf("%s‚ÍŠJ‚¯‚Ü‚¹‚ñ\n", filename);
    return -1;
  }

  readSize = fread(signature, 1, SIGNATURE_NUM, fi);

  if(png_sig_cmp(signature, 0, SIGNATURE_NUM)){
    printf("png_sig_cmp error!\n");
    fclose(fi);
    return -1;
  }

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(png == NULL){
    printf("png_create_read_struct error!\n");
    fclose(fi);
    return -1;
  }

  info = png_create_info_struct(png);
  if(info == NULL){
    printf("png_crete_info_struct error!\n");
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fi);
    return -1;
  }

  png_init_io(png, fi);
  png_set_sig_bytes(png, readSize);
  png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);

  datap = png_get_rows(png, info);

  type = png_get_color_type(png, info);
  /* ‚Æ‚è‚ ‚¦‚¸RGB or RGBA‚¾‚¯‘Î‰ž */
  if(type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGB_ALPHA){
    printf("color type is not RGB or RGBA\n");
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fi);
    return -1;
  }

  bitmapData->width = width;
  bitmapData->height = height;
  if(type == PNG_COLOR_TYPE_RGB) {
    bitmapData->ch = 3;
  } else if(type == PNG_COLOR_TYPE_RGBA) {
    bitmapData->ch = 4;
  }
  printf("width = %d, height = %d, ch = %d\n", bitmapData->width, bitmapData->height, bitmapData->ch);

  bitmapData->data =
    (unsigned char*)malloc(sizeof(unsigned char) * bitmapData->width * bitmapData->height * bitmapData->ch);
  if(bitmapData->data == NULL){
    printf("data malloc error\n");
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fi);
    return -1;
  }

  for(j = 0; j < bitmapData->height; j++){
    memcpy(bitmapData->data + j * bitmapData->width * bitmapData->ch, datap[j], bitmapData->width * bitmapData->ch);
  }

  png_destroy_read_struct(&png, &info, NULL);
  fclose(fi);

  return 0;
}

int pngFileEncodeWrite(BITMAPDATA_t *bitmapData, const char *filename){
  FILE *fo;
  int j;

  png_structp png;
  png_infop info;
  png_bytepp datap;
  png_byte type;

  fo = fopen(filename, "wb");
  if(fo == NULL){
    printf("%s‚ÍŠJ‚¯‚Ü‚¹‚ñ\n", filename);
    return -1;
  }

  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(png);

  if(bitmapData->ch == 3) {
    type = PNG_COLOR_TYPE_RGB;
  } else if(bitmapData->ch == 4) {
    type = PNG_COLOR_TYPE_RGB_ALPHA;
  } else {
    printf("ch num is invalid!\n");
    png_destroy_write_struct(&png, &info);
    fclose(fo);
    return -1;
  }
  png_init_io(png, fo);
  png_set_IHDR(png, info, bitmapData->width, bitmapData->height, 8, type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  datap = png_malloc(png, sizeof(png_bytep) * bitmapData->height);

  png_set_rows(png, info, datap);

  for(j = 0; j < bitmapData->height; j++){
    datap[j] = png_malloc(png, bitmapData->width * bitmapData->ch);
    memcpy(datap[j], bitmapData->data + j * bitmapData->width * bitmapData->ch, bitmapData->width * bitmapData->ch);
  }
  png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

  for(j = 0; j < bitmapData->height; j++){
    png_free(png, datap[j]);
  }
  png_free(png, datap);

  png_destroy_write_struct(&png, &info);
  fclose(fo);
  return 0;
}


int freeBitmapData(BITMAPDATA_t *bitmap){
  if(bitmap->data != NULL){
    free(bitmap->data);
    bitmap->data = NULL;
  }
  return 0;
}