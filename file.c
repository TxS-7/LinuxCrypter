#include "file.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static long get_file_size(FILE *fp);

uint8_t *read_file(const char *filename, size_t *bytes_read)
{
	*bytes_read = 0;

	if (!file_exists(filename)) {
		return NULL;
	}

	FILE *fp = fopen(filename, "rb");
	long file_size = get_file_size(fp);
	if (file_size <= 0) {
		fclose(fp);
		return NULL;
	}

	uint8_t *buf = malloc(file_size);
	size_t read_size = fread(buf, 1, file_size, fp);
	
	if (read_size == 0 || read_size < file_size) {
		fprintf(stderr, "read_file: Something went wrong while reading file: %s\n", filename);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	*bytes_read = read_size;
	return buf;
}

int write_to_file_at_offset(const char *filename, const uint8_t *buf, size_t buf_size, long offset)
{
	if (!file_exists(filename)) {
		return -1;
	}

	FILE *fp = fopen(filename, "rb+");
	long file_size = get_file_size(fp);
	if (offset >= file_size) {
		fclose(fp);
		return -1;
	}

	if (fseek(fp, offset, SEEK_SET) == -1) {
		fprintf(stderr, "fseek");
		fclose(fp);
		return -1;
	}

	size_t bytes_written = fwrite(buf, 1, buf_size, fp);
	if (bytes_written != buf_size) {
		fprintf(stderr, "fwrite");
		fclose(fp);
		return -1;
	}

	fclose(fp);

	return 0;
}

int file_exists(const char *filename)
{
	return access(filename, F_OK) != -1;
}

static long get_file_size(FILE *fp)
{
	if (fseek(fp, 0, SEEK_END) == -1) {
		return -1;
	}

	long file_size = ftell(fp);
	rewind(fp);

	return file_size;
}
