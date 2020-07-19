#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>

extern "C" {
	#include "file.h"
	#include <stdint.h>
	#include <stddef.h>
	#include <string.h>
}

static bool test_file_exists(const char *filename)
{
	return access(filename, F_OK) != -1;
}

static bool test_files_are_the_same(const char *filename1, const char *filename2)
{
	const std::string filename1_str(filename1);
	const std::string filename2_str(filename2);

	std::ifstream file1(filename1_str, std::ios::binary);
	std::ifstream file2(filename2_str, std::ios::binary);

	file1.seekg(0, std::ios::end);
	std::streampos file1_size = file1.tellg();
	file1.seekg(0, std::ios::beg);

	file2.seekg(0, std::ios::end);
	std::streampos file2_size = file2.tellg();
	file2.seekg(0, std::ios::beg);

	if (file1_size != file2_size) {
		return false;
	}

	char file1_contents[64];
	file1.read(file1_contents, file1_size);
	char file2_contents[64];
	file2.read(file2_contents, file2_size);

	return memcmp(file1_contents, file2_contents, file1_size) == 0;
}

static void copy_test_file(const char *src_filename, const char *dest_filename)
{
	std::ifstream src(src_filename, std::ios::binary);
    std::ofstream dest(dest_filename, std::ios::binary);

    dest << src.rdbuf();

    src.close();
    dest.close();
}

TEST(FileTests, test_read_file)
{
	const char *filename = "../../tests/testfiles/file_tests/example1.txt";
	const uint8_t expected_contents[] = { 'A', 'B', 'C', 'D', 'E' };

	size_t read_size;
	const uint8_t *buf = read_file(filename, &read_size);

	ASSERT_NE(nullptr, buf);
	ASSERT_EQ(5u, read_size);

	ASSERT_TRUE(0 == memcmp(buf, expected_contents, sizeof(expected_contents)));
}

TEST(FileTests, test_read_file_binary_data)
{
	const char *filename = "../../tests/testfiles/file_tests/example2.txt";
	const uint8_t expected_contents[] = { 0x00, 0xfe, 0x33, 0xc2, 0x3d, 0xaa, 0xbc, 0x00 };

	size_t read_size;
	const uint8_t *buf = read_file(filename, &read_size);

	ASSERT_NE(nullptr, buf);
	ASSERT_EQ(8u, read_size);

	ASSERT_TRUE(0 == memcmp(buf, expected_contents, sizeof(expected_contents)));
}

TEST(FileTests, test_read_file_file_does_not_exist)
{
	const char *filename = "../../tests/testfiles/file_tests/not_found.txt";

	size_t read_size;
	const uint8_t *buf = read_file(filename, &read_size);

	ASSERT_EQ(nullptr, buf);
	ASSERT_EQ(0u, read_size);
}

TEST(FileTests, test_read_file_file_is_null)
{
	size_t read_size;
	const uint8_t *buf = read_file(NULL, &read_size);

	ASSERT_EQ(nullptr, buf);
	ASSERT_EQ(0u, read_size);
}

TEST(FileTests, test_read_file_file_is_empty)
{
	const char *filename = "../../tests/testfiles/file_tests/empty.txt";

	size_t read_size;
	const uint8_t *buf = read_file(filename, &read_size);

	ASSERT_EQ(nullptr, buf);
	ASSERT_EQ(0u, read_size);
}

TEST(FileTests, test_write_to_file_at_offset_new_file_write_at_beginning)
{
	const char *filename = "../../tests/testfiles/file_tests/output/new_file1.txt";
	const char *expected_output_filename = "../../tests/testfiles/file_tests/expected/new_file1.txt";

	ASSERT_FALSE(test_file_exists(filename));

	const uint8_t buf[] = { 0x00, 0xfe, 0x33, 0xc2, 0x3d, 0xaa, 0xbc, 0x00 };

	ASSERT_EQ(0, write_to_file_at_offset(filename, buf, sizeof(buf), 0));

	ASSERT_TRUE(test_files_are_the_same(filename, expected_output_filename));

	remove(filename);
}

TEST(FileTests, test_write_to_file_at_offset_new_file_write_at_random_offset)
{
	const char *filename = "../../tests/testfiles/file_tests/output/new_file2.txt";
	const char *expected_output_filename = "../../tests/testfiles/file_tests/expected/new_file2.txt";

	ASSERT_FALSE(test_file_exists(filename));

	const uint8_t buf[] = { 0x00, 0xfe, 0x33, 0xc2, 0x3d, 0xaa, 0xbc, 0x00 };

	ASSERT_EQ(0, write_to_file_at_offset(filename, buf, sizeof(buf), 5));

	ASSERT_TRUE(test_files_are_the_same(filename, expected_output_filename));

	remove(filename);
}

TEST(FileTests, test_write_to_file_at_offset_existing_file_write_at_beginning)
{
	const char *src_filename = "../../tests/testfiles/file_tests/existing.txt";
	const char *filename = "../../tests/testfiles/file_tests/output/existing1.txt";
	const char *expected_output_filename = "../../tests/testfiles/file_tests/expected/existing1.txt";

	ASSERT_FALSE(test_file_exists(filename));
	copy_test_file(src_filename, filename);
	ASSERT_TRUE(test_file_exists(filename));

	const uint8_t buf[] = { 0x00, 0xfe, 0x33, 0xc2, 0x3d, 0xaa, 0xbc, 0x00 };

	ASSERT_EQ(0, write_to_file_at_offset(filename, buf, sizeof(buf), 0));

	ASSERT_TRUE(test_files_are_the_same(filename, expected_output_filename));

	remove(filename);
}

TEST(FileTests, test_write_to_file_at_offset_existing_file_write_at_random_offset)
{
	const char *src_filename = "../../tests/testfiles/file_tests/existing.txt";
	const char *filename = "../../tests/testfiles/file_tests/output/existing2.txt";
	const char *expected_output_filename = "../../tests/testfiles/file_tests/expected/existing2.txt";

	ASSERT_FALSE(test_file_exists(filename));
	copy_test_file(src_filename, filename);
	ASSERT_TRUE(test_file_exists(filename));

	const uint8_t buf[] = { 0x00, 0xfe, 0x33, 0xc2 };

	ASSERT_EQ(0, write_to_file_at_offset(filename, buf, sizeof(buf), 5));

	ASSERT_TRUE(test_files_are_the_same(filename, expected_output_filename));

	remove(filename);
}

TEST(FileTests, test_write_to_file_at_offset_filename_is_null)
{
	const uint8_t buf[] = { 0x00, 0xfe, 0x33, 0xc2, 0x3d, 0xaa, 0xbc, 0x00 };

	ASSERT_EQ(-1, write_to_file_at_offset(NULL, buf, sizeof(buf), 5));
}

TEST(FileTests, test_write_to_file_at_offset_buf_is_null)
{
	const char *filename = "../../tests/testfiles/file_tests/output/fail1.txt";

	ASSERT_FALSE(test_file_exists(filename));

	ASSERT_EQ(-1, write_to_file_at_offset(filename, NULL, 8u, 5));

	ASSERT_FALSE(test_file_exists(filename));
}

TEST(FileTests, test_write_to_file_at_offset_buf_size_is_zero)
{
	const char *filename = "../../tests/testfiles/file_tests/output/fail2.txt";

	ASSERT_FALSE(test_file_exists(filename));

	const uint8_t buf[] = { 0x00, 0xfe, 0x33, 0xc2, 0x3d, 0xaa, 0xbc, 0x00 };

	ASSERT_EQ(-1, write_to_file_at_offset(filename, buf, 0u, 5));

	ASSERT_FALSE(test_file_exists(filename));
}

TEST(FileTests, test_write_to_file_at_offset_offset_is_negative)
{
	const char *filename = "../../tests/testfiles/file_tests/output/fail3.txt";

	ASSERT_FALSE(test_file_exists(filename));

	const uint8_t buf[] = { 0x00, 0xfe, 0x33, 0xc2, 0x3d, 0xaa, 0xbc, 0x00 };

	ASSERT_EQ(-1, write_to_file_at_offset(filename, buf, sizeof(buf), -5));

	ASSERT_FALSE(test_file_exists(filename));
}

TEST(FileTests, test_copy_file)
{
	const char *src_filename = "../../tests/testfiles/file_tests/existing.txt";
	const char *dest_filename = "../../tests/testfiles/file_tests/output/copy1.txt";

	ASSERT_FALSE(test_file_exists(dest_filename));

	ASSERT_EQ(0, copy_file(src_filename, dest_filename));

	ASSERT_TRUE(test_files_are_the_same(src_filename, dest_filename));

	remove(dest_filename);
}

TEST(FileTests, test_copy_file_replace_existing_file)
{
	const char *src_filename = "../../tests/testfiles/file_tests/existing.txt";
	const char *dest_filename = "../../tests/testfiles/file_tests/output/copy2.txt";
	const char *file_to_overwrite = "../../tests/testfiles/file_tests/existing2.txt";

	ASSERT_FALSE(test_file_exists(dest_filename));
	copy_test_file(file_to_overwrite, dest_filename);
	ASSERT_TRUE(test_file_exists(dest_filename));

	ASSERT_FALSE(test_files_are_the_same(src_filename, dest_filename));

	ASSERT_EQ(0, copy_file(src_filename, dest_filename));

	ASSERT_TRUE(test_files_are_the_same(src_filename, dest_filename));

	remove(dest_filename);
}

TEST(FileTests, test_copy_file_source_file_does_not_exist)
{
	const char *src_filename = "../../tests/testfiles/file_tests/not_found.txt";
	const char *dest_filename = "../../tests/testfiles/file_tests/output/copy_fail1.txt";

	ASSERT_FALSE(test_file_exists(dest_filename));

	ASSERT_EQ(-1, copy_file(src_filename, dest_filename));

	ASSERT_FALSE(test_file_exists(dest_filename));
}

TEST(FileTests, test_copy_file_source_filename_is_null)
{
	const char *dest_filename = "../../tests/testfiles/file_tests/output/copy_fail2.txt";

	ASSERT_FALSE(test_file_exists(dest_filename));

	ASSERT_EQ(-1, copy_file(NULL, dest_filename));

	ASSERT_FALSE(test_file_exists(dest_filename));
}

TEST(FileTests, test_copy_file_destination_filename_is_null)
{
	const char *src_filename = "../../tests/testfiles/file_tests/existing.txt";

	ASSERT_EQ(-1, copy_file(src_filename, NULL));
}

TEST(FileTests, test_file_exists)
{
	const char *filename = "../../tests/testfiles/file_tests/example1.txt";

	ASSERT_TRUE(file_exists(filename));
}

TEST(FileTests, test_file_exists_file_does_not_exist)
{
	const char *filename = "../../tests/testfiles/file_tests/not_found.txt";

	ASSERT_FALSE(file_exists(filename));
}

TEST(FileTests, test_file_exists_filename_is_null)
{
	ASSERT_FALSE(file_exists(NULL));
}