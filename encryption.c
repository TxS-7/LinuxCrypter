#include "encryption.h"
#include <stdlib.h>

uint8_t *encrypt(const uint8_t *buf, size_t buf_size, const uint8_t *key, size_t key_size, size_t *encrypted_buf_size)
{
	*encrypted_buf_size = 0;

	if (!buf || !key) {
		return NULL;
	}

	if (buf_size == 0 || key_size == 0) {
		return NULL;
	}

	uint8_t *encrypted_buf = malloc(buf_size);
	for (size_t i = 0; i < buf_size; i++) {
		encrypted_buf[i] = buf[i] ^ key[i % key_size];
	}

	*encrypted_buf_size = buf_size;
	return encrypted_buf;
}

uint8_t *decrypt(const uint8_t *buf, size_t buf_size, const uint8_t *key, size_t key_size, size_t *decrypted_buf_size)
{
	return encrypt(buf, buf_size, key, key_size, decrypted_buf_size);
}
