#ifndef CRYPTER_H
#define CRYPTER_H

#include <stdint.h>

#define SUCCESS 0
#define FAIL 1
#define CRYPTER_MAX_PAYLOAD_SIZE 100000 // 100 KB

extern const uint8_t g_encryption_key[8];

int encrypt_file(const char *payload_filename, const char *stub_filename);

#endif // CRYPTER_H
