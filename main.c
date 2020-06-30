#include "crypter.h"

int main(void)
{
	encrypt_file("/etc/passwd", "stub.c");
	return 0;
}
