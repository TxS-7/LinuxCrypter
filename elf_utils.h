#ifndef ELF_UTILS_H
#define ELF_UTILS_H

#include <stdint.h>
#include <stddef.h>

int is_elf(const uint8_t *file_content, size_t file_size);

#endif // ELF_UTILS_H
