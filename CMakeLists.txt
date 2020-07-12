cmake_minimum_required(VERSION 2.8)

project(Crypter)

add_executable(crypter main.c crypter.c encryption.c elf_utils.c file.c)
add_executable(stub stub.c crypter.c encryption.c elf_utils.c file.c)
add_executable(hello hello.c)
