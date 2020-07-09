#include "crypter.h"
#include "file.h"
#include "encryption.h"
#include "elf_utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const uint8_t g_encryption_key[8] = { 0xdf, 0x4e, 0xee, 0x12, 0xfe, 0xc3, 0x56, 0x09 };

static uint8_t *get_encrypted_bytes(const char *filename, size_t *encrypted_size);
static int write_to_stub(const uint8_t *buf, size_t buf_size, const char *stub_filename);
static long find_in_buffer(const uint8_t *buf, size_t buf_size, const uint8_t *bytes_to_find, size_t bytes_to_find_size);

int encrypt_file(const char *payload_filename, const char *stub_filename)
{
	if (!payload_filename || !file_exists(payload_filename)) {
		fprintf(stderr, "Invalid payload\n");
		return FAIL;
	}

	if (!stub_filename || !file_exists(stub_filename)) {
		fprintf(stderr, "Invalid stub\n");
		return FAIL;
	}

	size_t encrypted_payload_size;
	uint8_t *encrypted_payload = get_encrypted_bytes(payload_filename, &encrypted_payload_size);
	if (!encrypted_payload) {
		fprintf(stderr, "encrypt_file: Failed to encrypt file: %s\n", payload_filename);
		return FAIL;
	}

	if (encrypted_payload_size > CRYPTER_MAX_PAYLOAD_SIZE - sizeof(size_t)) {
		fprintf(stderr, "Encrypted payload is too large\n");
		return FAIL;
	}

	if (write_to_stub(encrypted_payload, encrypted_payload_size, stub_filename) == FAIL) {
		fprintf(stderr, "encrypt_file: Failed to write encrypted file to stub\n");
		return FAIL;
	}

	printf("Done\n");

	return SUCCESS;
}

static uint8_t *get_encrypted_bytes(const char *filename, size_t *encrypted_size)
{
	size_t buf_size;
	uint8_t *buf = read_file(filename, &buf_size);
	if (!buf) {
		return NULL;
	}

	printf("Read payload of size: %ld\n", buf_size);

	size_t key_size = sizeof(g_encryption_key);

	uint8_t *encrypted_payload = encrypt(buf, buf_size, g_encryption_key, key_size, encrypted_size);

	printf("Encrypted payload size is: %ld\n", *encrypted_size);

	return encrypted_payload;
}

static int write_to_stub(const uint8_t *buf, size_t buf_size, const char *stub_filename)
{
	size_t stub_size;
	uint8_t *stub_buf = read_file(stub_filename, &stub_size);
	if (!stub_buf) {
		fprintf(stderr, "Failed to read the stub: %s\n", stub_filename);
	}

	uint8_t bytes_to_find[] = { 'A', 'B', 'C', 'D', 'E', 'F' };
	long write_pos = find_in_buffer(stub_buf, stub_size, bytes_to_find, sizeof(bytes_to_find));
	if (write_pos == -1) {
		fprintf(stderr, "Failed to find position to write in the stub\n");
		return FAIL;
	}

	printf("Writing payload at position: %ld\n", write_pos);

	size_t buf_to_write_size = sizeof(size_t) + buf_size;
	uint8_t *buf_to_write = malloc(buf_to_write_size);

	*(size_t *) buf_to_write = buf_size;

	memcpy(buf_to_write + sizeof(size_t), buf, buf_size);

	if (write_to_file_at_offset(stub_filename, buf_to_write, buf_to_write_size, write_pos) == -1) {
		fprintf(stderr, "Failed to write the payload to the stub: %s\n", stub_filename);
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
