CC = gcc

INCLUDES = -IHorseEFI/include

CFLAGS = -ffreestanding -c -Wall -fshort-wchar -fno-stack-protector -fpic -mno-red-zone
LFLAGS = -nostdlib -nodefaultlibs -shared -Bsymbolic -znocombreloc

SRC = $(wildcard **/*.c)


all:
	$(CC) $(CFLAGS) $(INCLUDES) -o main.o src/main.c
	$(CC) $(CFLAGS) $(INCLUDES) -o efi.o HorseEFI/src/efi.c
	$(CC) $(LFLAGS) -e efi_main -o bootx64.efi main.o efi.o