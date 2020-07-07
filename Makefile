CRYPTER_OBJS = main.o crypter.o encryption.o elf_utils.o file.o
STUB_OBJS = stub.o crypter.o encryption.o elf_utils.o file.o
CRYPTER_EXEC = crypter
STUB_EXEC = stub

all: crypter stub

crypter: ${CRYPTER_OBJS}
	gcc -o ${CRYPTER_EXEC} ${CRYPTER_OBJS}

stub: ${STUB_OBJS}
	gcc -o ${STUB_EXEC} ${STUB_OBJS}

main.o: main.c crypter.h
	gcc -Wall -c main.c

stub.o: stub.c crypter.h elf_utils.h encryption.h
	gcc -Wall -c stub.c

crypter.o: crypter.c crypter.h elf_utils.h encryption.h file.h
	gcc -Wall -c crypter.c

encryption.o: encryption.c encryption.h
	gcc -Wall -c encryption.c

elf_utils.o: elf_utils.c elf_utils.h
	gcc -Wall -c elf_utils.c

file.o: file.c file.h
	gcc -Wall -c file.c

hello: hello.c
	gcc -o hello hello.c

clean:
	rm -f ${CRYPTER_OBJS} ${STUB_OBJS} ${CRYPTER_EXEC} ${STUB_EXEC}
