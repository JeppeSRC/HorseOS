CC = clang


INCLUDES = -IHorseEFI/include

CFLAGS = -ffreestanding -c -Wall -fshort-wchar -fno-stack-protector -mno-red-zone -target x86_64-pc-win32-coff -O3 -x c -std=c11
LFLAGS = -nostdlib -nodefaultlibs -target x86_64-pc-win32-coff -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link

SRC = $(wildcard **/*.c)


all:
	$(CC) $(CFLAGS) $(INCLUDES) -o main.o src/main.c
	$(CC) $(CFLAGS) $(INCLUDES) -o efi.o HorseEFI/src/efi.c
	$(CC) $(LFLAGS) -o bootx64.efi main.o efi.o