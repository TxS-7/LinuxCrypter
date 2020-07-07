# Simple Linux Crypter

This is a Simple Linux Crypter used to encrypt ELF files.

The crypter works by encrypting the input ELF file using XOR encryption with a predefined key.
The encrypted payload is then written to the stub (stub.c).

The stub reads the encrypted payload from its memory, decrypts it back to an ELF file
and executes it in memory.


## How It Works

In the stub we define an initialized global buffer that will allocate space in the .data section of the
resulting ELF file. This is where the encrypted payload will be written by the crypter application.

The buffer has a 100KB limit defined in crypter.h (CRYPTER\_MAX\_PAYLOAD\_SIZE) and it starts with "ABCDEF"
so that the crypter can find the location to write.

When the stub application runs, it assumes that the buffer has been replaced by the payload and its size.

After the stub decrypts the payload and produces the original ELF file, it opens and executes it in memory using
memfd_create (Linux kernel version >= 3.17 required) without creating a file in the filesystem.

The name of the process after the payload is run is defined in stub.c.

The usage of memfd_create from the process can still be seen using the lsof command:
```
$ lsof -p <pid>
.............................. /memfd:myfile (deleted)
```


## How To Build It

```
$ make crypter
$ make stub
```


## How To Test It

To test it with the hello world application (hello.c):

1. Build hello.c:
	```
	$ make hello
	```
2. Run the crypter:
	```
	$ ./crypter hello stub
	Read payload of size: 16728
	Encrypted payload size is: 16728
	Writing payload at position: 12320
	Done
	```
3. Run the modified stub now containing the hello world application:
	```
	$ ./stub
	Read encrypted payload of size: 16688
	Decrypted ELF size is: 16688
	RUNNING...
	\
	Hello world!
	```