#include "elf_utils.h"
#include <string.h>

int is_elf(const uint8_t *file_content, size_t file_size)
{
	const uint8_t elf_magic_number[] = { 0x7f, 'E', 'L', 'F' };

	if (!file_content || file_size < sizeof(elf_magic_number)) {
		return 0;
	}

	return memcmp(file_content, elf_magic_number, sizeof(elf_magic_number)) == 0;
}