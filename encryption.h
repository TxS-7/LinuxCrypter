#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdint.h>
#include <stddef.h>

/* Simple XOR encryption */

uint8_t *encrypt(uint8_t *buf, size_t buf_size, uint8_t *key, size_t key_size, size_t *encrypted_buf_size);
uint8_t *decrypt(uint8_t *buf, size_t buf_size, uint8_t *key, size_t key_size, size_t *decrypted_buf_size);

#endif // ENCRYPTION_H
