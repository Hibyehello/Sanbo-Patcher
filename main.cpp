#include "FIFO.h"
#include "janpatch.h"
#include <stdio.h>
#include <stdlib.h>

size_t write(const void *ptr, size_t size_dummy, size_t count, FILE *file);

FIFO buffer;

int main()
{
  buffer.size = 0xf;

  buffer.buffer = (u8 *)malloc(sizeof(u8) * buffer.size);
  

  // printf("%d\n", buffer.size);

  // for (int i = 0; i <= 0xff; i++)
  // {
  //   buffer.write((void *)&i, sizeof(u8), 1, buffer.bin);
  //   printf("%d\n", i);
  // }

  // buffer.flush();

  // fclose(buffer.bin);
  // Open streams
  FILE *sanbo_bin = fopen("main1", "rb+");
  FILE *patch = fopen("patch.diff", "rb");
  //FILE *temp = fopen("patched.txt", "wb");

  // janpatch_ctx contains buffers, and references to the file system functions
  janpatch_ctx ctx = {{(unsigned char *)malloc(1024), 1024}, // source buffer
                      {(unsigned char *)malloc(1024), 1024}, // patch buffer
                      {(unsigned char *)malloc(1024), 1024}, // target buffer

                      &fread,
                      &write,
                      &fseek,
                      &ftell};

  int ret = janpatch(ctx, sanbo_bin, patch, sanbo_bin);
  buffer.flush(sanbo_bin);

  return ret;
}

size_t write(const void *ptr, size_t size_dummy, size_t count, FILE *file)
{
  return buffer.write(ptr, size_dummy, count, file);
}