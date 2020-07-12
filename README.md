# Simple Linux Crypter

This is a Simple Linux Crypter used to encrypt ELF files.

The crypter works by encrypting the input ELF file using XOR encryption with a predefined key.
The encrypted payload is then written to the stub's .data section (stub.c).

The stub reads the encrypted payload from its memory, decrypts it back to an ELF file and executes it.


## How It Works

In the stub we define an initialized global buffer that will allocate space in the .data section of the
resulting ELF file. This is where the encrypted payload will be written by the crypter application.

The buffer has a 100KB limit defined in crypter.h (`CRYPTER_MAX_PAYLOAD_SIZE`) and it starts with "ABCDEF"
so that the crypter can find the location to write.

When the stub application runs, it assumes that the buffer has been replaced by the payload and its size.

After the stub decrypts the payload and produces the original ELF file, it opens and executes it in memory using
`memfd_create` (Linux kernel version >= 3.17 required) without creating a file in the filesystem.

**Note:** After `memfd_create`, a new file descriptor is created under `/proc/<pid>/fd/<fd>`. When we write the decrypted
	payload to that FD. From there, the decrypted payload can be read for a while. After the `execl` call, the FD will close due to the `MFD_CLOEXEC` flag.

The name of the process after the payload is run is defined in stub.c (`PROCESS_NAME`).

The usage of `memfd_create` from the process can still be seen using the `lsof` command:
```
$ lsof -p <pid>
.............................. /memfd:myfile (deleted)
```


## How To Build It

```
$ cd build/
$ cmake ..
$ make
```


## How To Test It

To test it with the hello world program (hello.c):

1. Run the crypter:
	```
	$ cd build/
	$ ./crypter hello stub output
	Read payload of size: 16688
	Encrypted payload size is: 16688
	Creating new file: output
	Writing payload to output at position: 16416
	Generated file: output
	```
2. Run the generated file containing the encrypted hello world program:
	```
	$ ./output
	Read encrypted payload of size: 16688
	Decrypted ELF size is: 16688
	RUNNING...
	\
	Hello world!
	```