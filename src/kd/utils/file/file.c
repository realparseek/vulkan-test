#include <kd/utils/file/file.h>

#include <stdio.h>

uint32_t kd_file_size(const char* path, kd_file_type type) {
  switch(type) {
    case KD_FILE_TYPE_BINARY: { 
      FILE* stdfile = fopen(path, "rb+");
      if (!stdfile) {
        puts("failed to size of binary file (filepath not found)");
      }
      uint32_t filesize = 0;
      fseek(stdfile, 0, SEEK_END);
      filesize = (uint32_t)ftell(stdfile);
      fseek(stdfile, 0, SEEK_SET);
      fclose(stdfile);
      return filesize;
      break;
    }
    default:
    case KD_FILE_TYPE_TEXT: { 
      FILE* stdfile = fopen(path, "rt+");
      if (!stdfile) {
        puts("failed to size of text file (filepath not found)");
      }
      uint32_t filesize = 0;
      fseek(stdfile, 0, SEEK_END);
      filesize = (uint32_t)ftell(stdfile);
      fseek(stdfile, 0, SEEK_SET);
      fclose(stdfile);
      return filesize;
      break;
    }
  }
}

void kd_file_read(const char* path, kd_file_type type, char* buffer, uint32_t size) {
  switch(type) {
    case KD_FILE_TYPE_BINARY: { 
      FILE* stdfile = fopen(path, "rb+");
      if (!stdfile) {
        puts("failed to read a binary file (filepath not found)");
      }
      fread(buffer, sizeof(char), size, stdfile);
      buffer[size] = 0;
      fclose(stdfile);
      break;
    }
    default:
    case KD_FILE_TYPE_TEXT: { 
      FILE* stdfile = fopen(path, "rt+");
      if (!stdfile) {
        puts("failed to read a text file (filepath not found)");
      }
      fread(buffer, sizeof(char), size, stdfile);
      buffer[size] = 0;
      fclose(stdfile);
      break;
    }
  }
}
