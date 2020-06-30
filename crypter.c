#include "crypter.h"
#include "file.h"
#include "encryption.h"
#include <stdio.h>
#include <stdint.h>

static uint8_t *get_encrypted_bytes(char *filename, size_t *encrypted_size);
static int write_to_stub(uint8_t *buf, size_t buf_size, char *stub_filename);

int encrypt_file(char *payload_filename, char *stub_filename)
{
	size_t encrypted_payload_size;
	uint8_t *encrypted_payload = get_encrypted_bytes(payload_filename, &encrypted_payload_size);
	if (!encrypted_payload) {
		fprintf(stderr, "encrypt_file: Failed to encrypt file: %s\n", payload_filename);
		return FAIL;
	}

	if (write_to_stub(encrypted_payload, encrypted_payload_size, stub_filename) == FAIL) {
		fprintf(stderr, "encrypt_file: Failed to write encrypted file to stub");
		return FAIL;
	}

	return SUCCESS;
}

static uint8_t *get_encrypted_bytes(char *filename, size_t *encrypted_size)
{
	size_t buf_size;
	uint8_t *buf = read_file(filename, &buf_size);
	if (!buf) {
		return NULL;
	}

	uint8_t key[] = { 0xdf, 0x4e, 0xee, 0x12, 0xfe, 0xc3, 0x56, 0x09 };
	size_t key_size = sizeof(key);

	uint8_t *encrypted_payload = encrypt(buf, buf_size, key, key_size, encrypted_size);

	return encrypted_payload;
}

static int write_to_stub(uint8_t *buf, size_t buf_size, char *stub_filename)
{
	return SUCCESS;
}
