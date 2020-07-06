#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stddef.h>

uint8_t *read_file(const char *filename, size_t *bytes_read);
int file_exists(const char *filename);

#endif // FILE_H
