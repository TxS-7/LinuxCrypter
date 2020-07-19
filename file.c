#include "file.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

static long get_file_size(FILE *fp);

uint8_t *read_file(const char *filename, size_t *bytes_read)
{
	*bytes_read = 0;

	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		perror("fopen");
		return NULL;
	}

	long file_size = get_file_size(fp);
	if (file_size <= 0) {
		fclose(fp);
		return NULL;
	}

	uint8_t *buf = malloc(file_size);
	if (!buf) {
		fprintf(stderr, "malloc: Failed to allocate %ld bytes\n", file_size);
		return NULL;
	}

	size_t read_size = fread(buf, 1, file_size, fp);

	if (read_size == 0 || read_size < file_size) {
		fprintf(stderr, "fread: Failed to read: %s\n", filename);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	*bytes_read = read_size;
	return buf;
}

int write_to_file_at_offset(const char *filename, const uint8_t *buf, size_t buf_size, long offset)
{
	if (!filename || !buf || buf_size == 0 || offset < 0) {
		return -1;
	}

	FILE *fp = NULL;

	if (!file_exists(filename)) {
		// Create a new file
		printf("Creating new file: %s\n", filename);
		fp = fopen(filename, "w");
	} else {
		fp = fopen(filename, "rb+");
	}

	if (!fp) {
		perror("fopen");
		return -1;
	}

	long file_size = get_file_size(fp);
	if (file_size < 0) {
		fclose(fp);
		return -1;
	}

	if (fseek(fp, offset, SEEK_SET) == -1) {
		perror("fseek");
		fclose(fp);
		return -1;
	}

	size_t bytes_written = fwrite(buf, 1, buf_size, fp);
	if (bytes_written != buf_size) {
		fprintf(stderr, "fwrite: Failed to write %ld bytes to %s\n", buf_size, filename);
		fclose(fp);
		return -1;
	}

	fclose(fp);

	return 0;
}

int copy_file(const char *src_filename, const char *dest_filename)
{
	if (!src_filename || !dest_filename) {
		return -1;
	}

	size_t buf_size;
	uint8_t *buf = read_file(src_filename, &buf_size);
	if (!buf) {
		fprintf(stderr, "Failed to read: %s\n", src_filename);
		return -1;
	}

	if (file_exists(dest_filename)) {
		if (remove(dest_filename) == -1) {
			perror("remove");
			free(buf);
			return -1;
		}
	}

	if (write_to_file_at_offset(dest_filename, buf, buf_size, 0) == -1) {
		fprintf(stderr, "Failed to write to: %s\n", dest_filename);
		free(buf);
		return -1;
	}

	free(buf);

	// Copy file permissions from source to destination
	struct stat src_stat;
	if (stat(src_filename, &src_stat) == -1) {
		perror("stat");
		return -1;
	}

	if (chmod(dest_filename, src_stat.st_mode) == -1) {
		perror("chmod");
		return -1;
	}

	return 0;
}

int file_exists(const char *filename)
{
	return access(filename, F_OK) != -1;
}

static long get_file_size(FILE *fp)
{
	if (fseek(fp, 0, SEEK_END) == -1) {
		perror("fseek");
		return -1;
	}

	long file_size = ftell(fp);
	if (file_size == -1) {
		perror("ftell");
		return -1;
	}

	rewind(fp);

	return file_size;
}
