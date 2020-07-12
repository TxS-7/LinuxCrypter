#include "crypter.h"
#include "file.h"
#include "encryption.h"
#include "elf_utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const uint8_t g_encryption_key[8] = { 0xdf, 0x4e, 0xee, 0x12, 0xfe, 0xc3, 0x56, 0x09 };

static uint8_t *encrypt_elf(const char *filename, size_t *encrypted_size);
static int generate_encrypted_file(const uint8_t *buf, size_t buf_size, const char *stub_filename, const char *output_filename);
static long find_in_buffer(const uint8_t *buf, size_t buf_size, const uint8_t *bytes_to_find, size_t bytes_to_find_size);

int encrypt_file(const char *payload_filename, const char *stub_filename, const char *output_filename)
{
	if (!payload_filename || !file_exists(payload_filename)) {
		fprintf(stderr, "Invalid payload\n");
		return FAIL;
	}

	if (!stub_filename || !file_exists(stub_filename)) {
		fprintf(stderr, "Invalid stub\n");
		return FAIL;
	}

	if (!output_filename) {
		fprintf(stderr, "Invalid output filename\n");
		return FAIL;
	}

	size_t encrypted_payload_size;
	uint8_t *encrypted_payload = encrypt_elf(payload_filename, &encrypted_payload_size);
	if (!encrypted_payload) {
		fprintf(stderr, "Failed to encrypt file: %s\n", payload_filename);
		return FAIL;
	}

	if (generate_encrypted_file(encrypted_payload, encrypted_payload_size, stub_filename, output_filename) == FAIL) {
		fprintf(stderr, "Failed to generate encrypted file\n");
		return FAIL;
	}

	free(encrypted_payload);

	return SUCCESS;
}

static uint8_t *encrypt_elf(const char *filename, size_t *encrypted_size)
{
	size_t buf_size;
	uint8_t *buf = read_file(filename, &buf_size);
	if (!buf) {
		return NULL;
	}

	printf("Read payload of size: %ld\n", buf_size);

	// Check if the payload is a valid ELF
	if (!is_elf(buf, buf_size)) {
		fprintf(stderr, "The payload is not a valid ELF\n");
		free(buf);
		return NULL;
	}

	size_t key_size = sizeof(g_encryption_key);

	uint8_t *encrypted_payload = encrypt(buf, buf_size, g_encryption_key, key_size, encrypted_size);
	if (!encrypted_payload) {
		fprintf(stderr, "Failed to encrypt the payload\n");
		free(buf);
		return NULL;
	}

	free(buf);

	printf("Encrypted payload size is: %ld\n", *encrypted_size);

	if (*encrypted_size > CRYPTER_MAX_PAYLOAD_SIZE - sizeof(size_t)) {
		fprintf(stderr, "Encrypted payload is too large\n");
		free(encrypted_payload);
		return NULL;
	}

	return encrypted_payload;
}

static int generate_encrypted_file(const uint8_t *buf, size_t buf_size, const char *stub_filename, const char *output_filename)
{
	size_t stub_size;
	uint8_t *stub_buf = read_file(stub_filename, &stub_size);
	if (!stub_buf) {
		fprintf(stderr, "Failed to read the stub: %s\n", stub_filename);
		return FAIL;
	}

	if (!is_elf(stub_buf, stub_size)) {
		fprintf(stderr, "The stub is not a valid ELF\n");
		free(stub_buf);
		return FAIL;
	}

	uint8_t bytes_to_find[] = { 'A', 'B', 'C', 'D', 'E', 'F' };
	long write_pos = find_in_buffer(stub_buf, stub_size, bytes_to_find, sizeof(bytes_to_find));
	if (write_pos == -1) {
		fprintf(stderr, "Failed to find position to write in the stub\n");
		free(stub_buf);
		return FAIL;
	}

	free(stub_buf);

	if (copy_file(stub_filename, output_filename) == -1) {
		return FAIL;
	}

	printf("Writing payload to %s at position: %ld\n", output_filename, write_pos);

	size_t buf_to_write_size = sizeof(size_t) + buf_size;
	uint8_t *buf_to_write = malloc(buf_to_write_size);

	*(size_t *) buf_to_write = buf_size;

	memcpy(buf_to_write + sizeof(size_t), buf, buf_size);

	if (write_to_file_at_offset(output_filename, buf_to_write, buf_to_write_size, write_pos) == -1) {
		free(buf_to_write);
		return FAIL;
	}

	free(buf_to_write);
	return SUCCESS;
}

static long find_in_buffer(const uint8_t *buf, size_t buf_size, const uint8_t *bytes_to_find, size_t bytes_to_find_size)
{
	if (buf_size == 0 || bytes_to_find_size == 0 || bytes_to_find_size > buf_size) {
		return -1;
	}

	for (size_t i = 0; i < buf_size - bytes_to_find_size; i++) {
		if (memcmp(buf + i, bytes_to_find, bytes_to_find_size) == 0) {
			return i;
		}
	}

	return -1;
}
