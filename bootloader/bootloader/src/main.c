#include <efi.h>

#define STR(x) case x: return L ## #x

CONST CHAR16* GetGraphicsPixelFormatString(EFI_GRAPHICS_PIXEL_FORMAT format) {
	switch (format) {
		STR(PixelRedGreenBlueReserved8BitPerColor);
		STR(PixelBlueGreenRedReserved8BitPerColor);
		STR(PixelBitMask);
		STR(PixelBltOnly);
	}

	return L"Error";
}


CONST CHAR16* GetMemoryTypeString(EFI_MEMORY_TYPE type) {
	switch (type) {
		STR(EfiReservedMemoryType);
		STR(EfiLoaderCode);
		STR(EfiLoaderData);
		STR(EfiBootServicesCode);
		STR(EfiBootServicesData);
		STR(EfiRuntimeServicesCode);
		STR(EfiRuntimeServicesData);
		STR(EfiConventionalMemory);
		STR(EfiUnusableMemory);
		STR(EfiACPIReclaimMemory);
		STR(EfiACPIMemoryNVS);
		STR(EfiMemoryMappedIO);
		STR(EfiMemoryMappedIOPortSpace);
		STR(EfiPalCode);
		STR(EfiPersistentMemory);
		STR(EfiMaxMemoryType);
	}

	return L"Error";
}

VOID* Allocate(UINTN bytes, EFI_MEMORY_TYPE type, EFI_BOOT_SERVICES* boot) {
	EFI_PHYSICAL_ADDRESS address = 0;
	UINTN pages = bytes / 4096 + (bytes % 4096 != 0 ? 1 : 0);
	EFI_STATUS status = boot->AllocatePages(AllocateAnyPages, type, pages, &address);

	if (status != EFI_SUCCESS) 
		return (VOID*)0;

	return (VOID*)address;
}

EFI_STATUS efi_main(EFI_HANDLE handle, EFI_SYSTEM_TABLE* systble) {
	InitializeLibrary(handle, systble);
	println(L"Booting....");

	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = 0;
	EFI_GUID guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

	systble->BootServices->LocateProtocol(&guid, 0, &gop);

	if (gop == 0) {
		println(L"Failed to locate EFI_GRAPHICS_OUTPUT_PROTOCOL!");
		WaitEscapeAndExit();
	}

	UINT32 width = 1920;
	UINT32 height = 0;

	EFI_GRAPHICS_PIXEL_FORMAT format = PixelRedGreenBlueReserved8BitPerColor;

	UINT32 gopModeIndex = GetGraphicsMode(gop, &width, &height, &format);

	if (format == PixelBltOnly) {
		println(L"Error: PixelBltOnly!");
		WaitEscapeAndExit();
	}

	gop->SetMode(gop, gopModeIndex);

	UINTN col = 0;
	UINTN row = 0;

	UINTN topModeIndex = GetTextMode(systble->ConOut, &col, &row);
	systble->ConOut->SetMode(systble->ConOut, topModeIndex);

	println(L"Booting...");
	printf(L"Setting framebuffer to mode %u:\n"
		   L"    Width x Height: %ux%u\n"
		   L"    Format:         %s\n"
		   L"    Address:        %H\n", gopModeIndex, width, height, GetGraphicsPixelFormatString(format), gop->Mode->FrameBufferBase);

	printf(L"Setting text mode to %U:\n    Columns x Rows: %Ux%U\n", topModeIndex, col, row);

	println(L"Loading memory map...");

	UINTN mapSize = 0;
	UINTN mapKey = 0;
	UINTN descSize = 0;
	UINT32 descVersion = 0;

	UINT8* desc = 0;
	
	systble->BootServices->GetMemoryMap(&mapSize, 0, &mapKey, &descSize, &descVersion);		// Getting memory map size
	mapSize += sizeof(EFI_MEMORY_DESCRIPTOR) * 64;											// Some extra space in case the map changes

	desc = Allocate(mapSize, EfiRuntimeServicesData, systble->BootServices);	//Allocating pages to store the memory map. Data in EfiRuntimeServicesData will be preserved when exiting bootservices and always available

	systble->BootServices->GetMemoryMap(&mapSize, (EFI_MEMORY_DESCRIPTOR*)desc, &mapKey, &descSize, &descVersion);	//Getting the actual memory map

	UINTN numEntries = mapSize / descSize;

	println(L"Memory Map");
	
	for (UINTN i = 0; i < 7; i++) {
		EFI_MEMORY_DESCRIPTOR d = *((EFI_MEMORY_DESCRIPTOR*)(desc + i * descSize));
		printf(L"Entry: %U\n", i);
		printf(L"   Type:           %s\n", GetMemoryTypeString(d.Type));
		printf(L"   Physical Start: %H\n", d.PhysicalStart);
		printf(L"   Virtual Start:  %H\n", d.VirtualStart);
		printf(L"   Size:           %H Bytes (%U Pages)\n", d.NumberOfPages * 4096, d.NumberOfPages);
	}

	WaitEscapeAndExit();

	return EFI_SUCCESS;
}