OBJS = main.o crypter.o encryption.o elf_utils.o file.o
EXEC = crypter

crypter: ${OBJS}
	gcc -o ${EXEC} ${OBJS}

main.o: main.c crypter.h
	gcc -Wall -c main.c

crypter.o: crypter.c crypter.h encryption.h file.h
	gcc -Wall -c crypter.c

encryption.o: encryption.c encryption.h
	gcc -Wall -c encryption.c

elf_utils.o: elf_utils.c elf_utils.h
	gcc -Wall -c elf_utils.c

file.o: file.c file.h
	gcc -Wall -c file.c

clean:
	rm -f ${OBJS} ${EXEC}
