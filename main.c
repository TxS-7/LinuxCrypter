#include <stdio.h>
#include "crypter.h"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <file to encrypt> <stub file>\n", argv[0]);
		return -1;
	}

	encrypt_file(argv[1], argv[2]);
	return 0;
}
