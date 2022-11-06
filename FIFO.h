#pragma once

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint32_t u32;

struct FIFO {
  u32 readIdx = 0;
  u32 writeIdx = 0;
  u32 f_ReadPos = 0;
  u32 f_WritePos = 0;
  u32 size;
  u8 *buffer;
  bool bufferFull = false;

  size_t write(const void *ptr, size_t size, size_t count, FILE *file);
  void flush(FILE* file);
};

int m_buffer[1024];
int m_loc = 0;

void writeAllFile(FILE* file, u32* writePos)
{
  fprintf(stderr, "%d\n", *writePos);
  fseek(file, *writePos, SEEK_SET);
  for(int i = 0; i < sizeof(m_buffer); i++)
    fwrite((void*)&m_buffer[i], 1, 1, file);
}

size_t FIFO::write(const void *ptr, size_t size_dummy, size_t count,
                   FILE *file) {
  u8* buf = (u8*)ptr;

  f_ReadPos = ftell(file);
  

  for (int i = 0; i < count; i++) {
    if (bufferFull) {
      m_buffer[m_loc++] = buffer[writeIdx];
      if(m_loc >= 1000) {
        m_loc = 0;
        writeAllFile(file, &f_WritePos);
        f_WritePos = ftell(file);
      }
    }


    buffer[writeIdx] = buf[i];

    writeIdx = (++writeIdx) % size;

    if (writeIdx == 0x0)
      bufferFull = true;
  }

  printf("Write: %d, Read: %d\n", f_WritePos, f_ReadPos);

  fseek(file, f_ReadPos, SEEK_SET);

  return size_dummy;
}

void FIFO::flush(FILE* file)
{
  fseek(file, f_WritePos, SEEK_SET);
  if (bufferFull) {
    for (int i = 0; i < size; i++) {
      fwrite((void*)&buffer[writeIdx], sizeof(u8), 1, file);
      writeIdx = (writeIdx + 1) % size;
    }
} else {
    for (int i = 0; i < writeIdx; i++) {
      fwrite((void*)&buffer[i], sizeof(u8), 1, file);
    }
}
}