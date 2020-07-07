#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stddef.h>

uint8_t *read_file(const char *filename, size_t *bytes_read);
int write_to_file_at_offset(const char *filename, const uint8_t *buf, size_t buf_size, long offset);
int file_exists(const char *filename);

#endif // FILE_H
