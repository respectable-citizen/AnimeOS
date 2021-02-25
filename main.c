#include <efi.h>
#include <efilib.h>

//EFI_SYSTEM_TABLE *ST;
//EFI_BOOT_SERVICES *BS;

EFI_STATUS status;

CHAR16 *INFO = L"Info";
CHAR16 *WARNING = L"Warning";
CHAR16 *ERROR = L"Error";

void hang() {
	for (;;);
}

void error_level_indicator(int error_level) {
    ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"[");
	if (error_level == 0) {
    	ST->ConOut->SetAttribute(ST->ConOut, EFI_BLUE);
    	ST->ConOut->OutputString(ST->ConOut, INFO);
	} else if (error_level == 1) {
    	ST->ConOut->SetAttribute(ST->ConOut, EFI_YELLOW);
    	ST->ConOut->OutputString(ST->ConOut, WARNING);
	} else {
    	ST->ConOut->SetAttribute(ST->ConOut, EFI_RED);
    	ST->ConOut->OutputString(ST->ConOut, ERROR);
	}
   	ST->ConOut->SetAttribute(ST->ConOut, EFI_WHITE);
    ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"] ");
}

void msg(CHAR16* msg, int error_level) {
	error_level_indicator(error_level);
	ST->ConOut->OutputString(ST->ConOut, msg);
    ST->ConOut->OutputString(ST->ConOut, (CHAR16*) L"\r\n");
}

EFI_MEMORY_DESCRIPTOR* get_memory_map(UINTN *map_key_out) {
	msg(L"Acquiring memory map", 0);

	EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
	UINT32 version = 0;
	UINTN map_key = 0;
	UINTN descriptor_size = 0;
	UINTN memory_map_size = 0;
	
	status = BS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &version);
	if (status == EFI_SUCCESS) {
		msg(L"First call to memory map succeeded (memory map is empty?)", 2);
	} else if (status == EFI_BUFFER_TOO_SMALL) {
		UINTN correct_size = memory_map_size + (2 * descriptor_size);
		error_level_indicator(0);
		Print(L"Memory map is %d bytes\r\n", correct_size);

		void *buffer = NULL;
		status = BS->AllocatePool(EfiLoaderData, correct_size, &buffer);
		if (status == EFI_SUCCESS) {
			memory_map = (EFI_MEMORY_DESCRIPTOR*) buffer;
			memory_map_size = correct_size;

			status = BS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &version);
			if (status == EFI_SUCCESS) {
			  /*char *mem = (char*) memory_map;
                Print(L"descriptor count:%d\n", (memory_map_size / descriptor_size));
                Print(L"descriptor_size:\t%d\n", descriptor_size);
                Print(L"EFI_MEMORY_DESCRIPTOR_VERSION = %d", version);
                for (int i = 0; i < (memory_map_size / descriptor_size); ++i) {
                    EFI_MEMORY_DESCRIPTOR desc = *( (EFI_MEMORY_DESCRIPTOR*) mem);
                    Print(L"\n");
                    Print(L"Physical Address of i-th memory descriptor:\t%x\n", desc.PhysicalStart);
                    Print(L"Virtual Address of i-th memory descriptor:\t%x\n", desc.VirtualStart);
                    Print(L"Memory Type of i-th memory descriptor:\t%d\n", desc.Type);
                    uefi_call_wrapper(BS->Stall, 1, 1000000);

                    mem += descriptor_size;
                }*/
				msg(L"Acquired memory map", 0);
				*map_key_out = map_key;
				return memory_map;
			} else {
				msg(L"Second GetMemoryMap call failed.", 2);
			}
		} else {
			msg(L"AllocatePool call failed", 0);
		}	
	} else {
		msg(L"First GetMemoryMap call failed.", 2);
	}
	return NULL;;
}

EFI_STATUS efi_main (EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *systab) {
    //ST = systab;
    //BS = systab->BootServices;
   	
	InitializeLib(image_handle, systab);

	ST->ConOut->ClearScreen(ST->ConOut);

	msg(L"Booting RetardOS", 0);

	UINTN map_key;
	EFI_MEMORY_DESCRIPTOR *memory_map = get_memory_map(&map_key);
	error_level_indicator(0);
	Print(L"Map key: %d\r\n", map_key);

	if (memory_map == NULL) hang();

	BS->ExitBootServices(image_handle, map_key);
	msg(L"Exited boot services", 0);

	for (;;);
	return EFI_SUCCESS;
}
