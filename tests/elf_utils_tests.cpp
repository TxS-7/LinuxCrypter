#include <gtest/gtest.h>

extern "C" {
	#include "elf_utils.h"
	#include <stdint.h>
}

TEST(ElfUtilsTests, test_is_elf)
{
    uint8_t buf[] = { 0x7f, 'E', 'L', 'F', 0x15, 0x04, 0xff, 0xf4};

    ASSERT_TRUE(is_elf(buf, sizeof(buf)));
}

TEST(ElfUtilsTests, test_is_elf_only_magic_numbers)
{
    uint8_t buf[] = { 0x7f, 'E', 'L', 'F' };

    ASSERT_TRUE(is_elf(buf, sizeof(buf)));
}

TEST(ElfUtilsTests, test_is_elf_invalid_elf_magic_numbers)
{
    uint8_t buf[] = { 0x7f, 'E', 'L', 'E', 0x15, 0x04, 0xff, 0xf4};

    ASSERT_FALSE(is_elf(buf, sizeof(buf)));
}

TEST(ElfUtilsTests, test_is_elf_incomplete_elf_magic_numbers)
{
    uint8_t buf[] = { 0x7f, 'E', 'L' };

    ASSERT_FALSE(is_elf(buf, sizeof(buf)));
}

TEST(ElfUtilsTests, test_is_elf_file_size_less_than_elf_magic_numbers_size)
{
    uint8_t buf[] = { 0x7f, 'E', 'L', 'F', 0x15, 0x04, 0xff, 0xf4};

    ASSERT_FALSE(is_elf(buf, 3));
}

TEST(ElfUtilsTests, test_is_elf_file_is_null)
{
    ASSERT_FALSE(is_elf(NULL, 1));
}
