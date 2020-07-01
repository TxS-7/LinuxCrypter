#ifndef CRYPTER_H
#define CRYPTER_H

#define SUCCESS 0
#define FAIL 1

int encrypt_file(const char *payload_filename, const char *stub_filename);

#endif // CRYPTER_H
