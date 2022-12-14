#include "FIFO.h"
#include "janpatch.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>

size_t write(const void *ptr, size_t size_dummy, size_t count, FILE *file);

FIFO buffer;

int main()
{
  buffer.size = 0x100000;
  buffer.buffer = (u8 *)malloc(sizeof(u8) * buffer.size);
  
  buffer.size2 = 0x100000;
  buffer.buffer2 = (u8 *)malloc(sizeof(u8) * buffer.size2);
  

  // printf("%d\n", buffer.size);

  // for (int i = 0; i <= 0xff; i++)
  // {
  //   buffer.write((void *)&i, sizeof(u8), 1, buffer.bin);
  //   printf("%d\n", i);
  // }

  // buffer.flush();

  // fclose(buffer.bin);
  // Open streams
  const char* sanbo_bin_path = "main1";
  FILE *sanbo_bin = fopen(sanbo_bin_path, "rb+");
  FILE *patch = fopen("patch.diff", "rb");
  //FILE *temp = fopen("patchedlipsum.txt", "wb");

  // janpatch_ctx contains buffers, and references to the file system functions
  janpatch_ctx ctx = {{(unsigned char *)malloc(0x100000), 0x100000}, // source buffer
                      {(unsigned char *)malloc(0x100000), 0x100000}, // patch buffer
                      {(unsigned char *)malloc(0x1000), 0x1000}, // target buffer

                      &fread,
                      &write,
                      &fseek,
                      &ftell};

  time_t startTime = time(0);
  int ret = janpatch(ctx, sanbo_bin, patch, sanbo_bin);
  buffer.flush(sanbo_bin);
  time_t endTime = time(0);
  printf("Ellapsed time: %llds\n", (long long)(endTime - startTime));

  truncate(sanbo_bin_path, buffer.f_WritePos);

  return ret;
}

size_t write(const void *ptr, size_t size_dummy, size_t count, FILE *file)
{
  return buffer.write(ptr, size_dummy, count, file);
}