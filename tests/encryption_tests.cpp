#include <gtest/gtest.h>

extern "C" {
	#include "encryption.h"
	#include <stdint.h>
	#include <stddef.h>
}

TEST(EncryptionTests, test_encrypt)
{
	uint8_t buf[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };
	uint8_t expected_encrypted_buf[] = { 0xbb, 0x99, 0xff, 0x99, 0xff, 0xdd };

	size_t result_size;
	uint8_t *result = encrypt(buf, sizeof(buf), key, sizeof(key), &result_size);

	ASSERT_NE(nullptr, result);
	ASSERT_EQ(6u, result_size);

	ASSERT_TRUE(0 == memcmp(result, expected_encrypted_buf, sizeof(expected_encrypted_buf)));
}

TEST(EncryptionTests, test_encrypt_buf_shorter_than_key)
{
	uint8_t buf[] = { 0xaa, 0xbb };
	uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };
	uint8_t expected_encrypted_buf[] = { 0xbb, 0x99 };

	size_t result_size;
	uint8_t *result = encrypt(buf, sizeof(buf), key, sizeof(key), &result_size);

	ASSERT_NE(nullptr, result);
	ASSERT_EQ(2u, result_size);

	ASSERT_TRUE(0 == memcmp(result, expected_encrypted_buf, sizeof(expected_encrypted_buf)));
}

TEST(EncryptionTests, test_encrypt_buf_is_null)
{
	uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };

	size_t result_size;
	uint8_t *result = encrypt(NULL, 10u, key, sizeof(key), &result_size);

	ASSERT_EQ(nullptr, result);
	ASSERT_EQ(0u, result_size);
}

TEST(EncryptionTests, test_encrypt_key_is_null)
{
	uint8_t buf[] = { 0xaa, 0xbb };

	size_t result_size;
	uint8_t *result = encrypt(buf, sizeof(buf), NULL, 10u, &result_size);

	ASSERT_EQ(nullptr, result);
	ASSERT_EQ(0u, result_size);
}

TEST(EncryptionTests, test_encrypt_buf_size_is_zero)
{
	uint8_t buf[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };
	uint8_t expected_encrypted_buf[] = { 0xbb, 0x99, 0xff, 0x99, 0xff, 0xdd };

	size_t result_size;
	uint8_t *result = encrypt(buf, 0u, key, sizeof(key), &result_size);

	ASSERT_EQ(nullptr, result);
	ASSERT_EQ(0u, result_size);
}

TEST(EncryptionTests, test_encrypt_key_size_is_zero)
{
	uint8_t buf[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };
	uint8_t expected_encrypted_buf[] = { 0xbb, 0x99, 0xff, 0x99, 0xff, 0xdd };

	size_t result_size;
	uint8_t *result = encrypt(buf, sizeof(key), key, 0u, &result_size);

	ASSERT_EQ(nullptr, result);
	ASSERT_EQ(0u, result_size);
}

TEST(EncryptionTests, test_decrypt)
{
	uint8_t buf[] = { 0xbb, 0x99, 0xff, 0x99, 0xff, 0xdd };
	uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };
	uint8_t expected_decrypted_buf[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

	size_t result_size;
	uint8_t *result = decrypt(buf, sizeof(buf), key, sizeof(key), &result_size);

	ASSERT_NE(nullptr, result);
	ASSERT_EQ(6u, result_size);

	ASSERT_TRUE(0 == memcmp(result, expected_decrypted_buf, sizeof(expected_decrypted_buf)));
}
