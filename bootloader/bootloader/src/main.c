#include <efi.h>

CONST CHAR16* GetGraphicsPixelFormatString(EFI_GRAPHICS_PIXEL_FORMAT format) {
	switch (format) {
	case PixelRedGreenBlueReserved8BitPerColor:
		return L"PixelRedGreenBlueReserved8BitPerColor";
	case PixelBlueGreenRedReserved8BitPerColor:
		return L"PixelBlueGreenRedReserved8BitPerColor";
	case PixelBitMask:
		return L"PixelBitMask";
	case PixelBltOnly:
		return L"PixelBltOnly";
	}

	return L"Error";
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
	gop->SetMode(gop, gopModeIndex);

	UINTN col = 0;
	UINTN row = 0;

	UINTN topModeIndex = GetTextMode(systble->ConOut, &col, &row);
	systble->ConOut->SetMode(systble->ConOut, topModeIndex);

	println(L"Booting...");
	printf(L"Setting framebuffer to mode %u:\n    Width x Height: %ux%u\n    Format: %s\n    Address: %H\n", gopModeIndex, width, height, GetGraphicsPixelFormatString(format), gop->Mode->FrameBufferBase);
	printf(L"Setting text mode to %U:\n    Columns x Rows: %Ux%U\n", topModeIndex, col, row);


	WaitEscapeAndExit();

	return EFI_SUCCESS;
}