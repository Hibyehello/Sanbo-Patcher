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
  
  u32 size2;
  u8 *buffer2;
  size_t buffer2pos = 0;

  size_t write2(const void *ptr, size_t size, size_t count, FILE *file);
  size_t write(const void *ptr, size_t size, size_t count, FILE *file);
  void flush(FILE* file);
  void flush2(FILE* file);
};

int m_buffer[1024];
int m_loc = 0;

// void writeAllFile(FILE* file, u32* writePos)
// {
//   fprintf(stderr, "%d\n", *writePos);
//   fseek(file, *writePos, SEEK_SET);
//   for(int i = 0; i < sizeof(m_buffer); i++)
//     fwrite((void*)&m_buffer[i], 1, 1, file);
// }

size_t FIFO::write2(const void *ptr, size_t size_dummy, size_t count,
                   FILE *file) {
    if (size_dummy * count > size2 - buffer2pos) {
        //Seek to the correct location
        f_ReadPos = ftell(file);
        if (f_ReadPos != f_WritePos) {
            fseek(file, f_WritePos, SEEK_SET);
        }
        
        //Write buffer and reset values
        fwrite((void*)buffer2, sizeof(u8), buffer2pos, file);
        f_WritePos += buffer2pos;
        buffer2pos = 0;
    }
    
    if (size_dummy * count > size2) {
        //Doesn't fit buffer, just write it to the file
        f_ReadPos = ftell(file);
        if (f_ReadPos != f_WritePos) {
            fseek(file, f_WritePos, SEEK_SET);
        }
        fwrite(ptr, size_dummy, count, file);
        f_WritePos += size_dummy * count;
    } else {
        memcpy(&buffer2[buffer2pos], ptr, size_dummy * count);
        buffer2pos += size_dummy * count;
    }
    
    
    return size_dummy * count;
}

size_t FIFO::write(const void *ptr, size_t size_dummy, size_t count,
                   FILE *file) {
  u8* buf = (u8*)ptr;
  //printf("%c\n", *(char*)ptr);

  for (int i = 0; i < count; i++) {
    if (bufferFull) {
      write2((void *)&buffer[writeIdx], sizeof(u8), 1, file);
      //printf("Writing %c at %d\n", buffer[writeIdx], f_WritePos);
    }

    //printf("%s", (char*)&buf[i]);

    buffer[writeIdx] = buf[i];

    writeIdx = (++writeIdx) % size;

    if (writeIdx == 0x0)
      bufferFull = true;
  }

  //fprintf(stderr, "Write: %d, Read: %d\n", f_WritePos, f_ReadPos);

  //TODO: Kevin has commented this out, uncomment if things are broken now, also see line 47
  //TODO: OK - Hib
  //f_WritePos = ftell(file);
  //fseek(file, f_ReadPos, 0);

  return size_dummy;
}

void FIFO::flush(FILE* file)
{
    fseek(file, f_WritePos, SEEK_SET);
    if (bufferFull) {
        for (int i = 0; i < size; i++) {
          write2((void*)&buffer[writeIdx], sizeof(u8), 1, file);
          writeIdx = (writeIdx + 1) % size;
        }
    } else {
        for (int i = 0; i < writeIdx; i++) {
          write2((void*)&buffer[i], sizeof(u8), 1, file);
        }
    }
    flush2(file);
}

void FIFO::flush2(FILE* file)
{
    //Seek to the correct location
    f_ReadPos = ftell(file);
    if (f_ReadPos != f_WritePos) {
        fseek(file, f_WritePos, SEEK_SET);
    }
    
    //Write buffer and reset values
    fwrite((void*)buffer2, sizeof(u8), buffer2pos, file);
    f_WritePos += buffer2pos;
    buffer2pos = 0;
    if (f_ReadPos != f_WritePos) {
        fseek(file, f_ReadPos, SEEK_SET);
    }
}