#pragma once

#include <stdint.h>

typedef enum kd_file_type {
  KD_FILE_TYPE_NONE = 0,
  KD_FILE_TYPE_TEXT,
  KD_FILE_TYPE_BINARY
} kd_file_type;

extern uint32_t kd_file_size(const char* path, kd_file_type type);

extern void kd_file_read(const char* path, kd_file_type type, char* buffer, uint32_t size);
