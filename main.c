#include <stdio.h>
#include "crypter.h"

int main(int argc, char *argv[])
{
	if (argc != 4) {
		printf("Usage: %s <file to encrypt> <stub file> <output file>\n", argv[0]);
		return -1;
	}

	if (encrypt_file(argv[1], argv[2], argv[3]) == FAIL) {
		printf("Crypter failed\n");
		return -1;
	}

	printf("Generated file: %s\n", argv[3]);
	return 0;
}
