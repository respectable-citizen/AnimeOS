#include <efi.h>
#include <protocol/efi-sfsp.h>
#include <protocol/efi-lip.h>
#include <protocol/efi-gop.h>

#include "elf.h"
#include "../common/kernel_arguments.h"

#define kernel_path L"\\EFI\\Custom\\kernel.elf"

EFI_BOOT_SERVICES *BS;
EFI_SYSTEM_TABLE *ST;
EFI_HANDLE IH;
EFI_STATUS status;

CHAR16* EFI_ERRORS[] = {
	L"EFI_LOAD_ERROR",
	L"EFI_INVALID_PARAMETER",
	L"EFI_UNSUPPORTED",
	L"EFI_BAD_BUFFER_SIZE",
	L"EFI_BUFFER_TOO_SMALL",
	L"EFI_NOT_READY",
	L"EFI_DEVICE_ERROR",
	L"EFI_WRITE_PROTECTED",
	L"EFI_OUT_OF_RESOURCES",
	L"EFI_VOLUME_CORRUPTED",
	L"EFI_VOLUME_FULL",
	L"EFI_NO_MEDIA",
	L"EFI_MEDIA_CHANGED",
	L"EFI_NOT_FOUND",
	L"EFI_ACCESS_DENIED",
	L"EFI_NO_RESPONSE",
	L"EFI_NO_MAPPING",
	L"EFI_TIMEOUT",
	L"EFI_NOT_STARTED",
	L"EFI_ALREADY_STARTED",
	L"EFI_ABORTED",
	L"EFI_ICMP_ERROR",
	L"EFI_TFTP_ERROR",
	L"EFI_PROTOCOL_ERROR",
	L"EFI_INCOMPATIBLE_VERSION",
	L"EFI_SECURITY_VIOLATION",
	L"EFI_CRC_ERROR",
	L"EFI_END_OF_MEDIA",
	L"EFI_END_OF_FILE",
	L"EFI_INVALID_LANGUAGE",
	L"EFI_COMPROMISED_DATA",
	L"EFI_IP_ADDRESS_CONFLICT",
	L"EFI_HTTP_ERROR"
};

typedef void(*KernelEntryPoint)(); 

void hang() {
	asm("cli");
	for (;;) asm("hlt");
}

void output_error() {
		ST->ConOut->OutputString(ST->ConOut, EFI_ERRORS[(status ^ EFI_ERR) - 1]);
		hang();
}

void output_number(uint64_t number) {
	if (number == 0) {
		ST->ConOut->OutputString(ST->ConOut, L"0");
		return;
	}

  char chars[60];
	chars[58] = '\0';
	chars[59] = '\0';

	int i = 56;
	for (;;) {
		int digit = number % 10;
		chars[i] = '0' + digit;
		chars[i + 1] = '\0';
		i -= 2;

		number /= 10;
		if (number == 0) break;
	}

	ST->ConOut->OutputString(ST->ConOut, (CHAR16*) (chars + i + 2));
}

void error_check(CHAR16 *name) {
	if (status != EFI_SUCCESS) {
		ST->ConOut->OutputString(ST->ConOut, name);
		ST->ConOut->OutputString(ST->ConOut, L" failed: ");
		output_error();
	}
}

MemoryMap get_memory_map() {
	EFI_MEMORY_DESCRIPTOR *memory_descriptors;
	UINT32 version = 0;
	UINTN map_key = 0;
	UINTN descriptor_size = 0;
	UINTN memory_map_size = 0;
	
	status = BS->GetMemoryMap(&memory_map_size, memory_descriptors, &map_key, &descriptor_size, &version);
	if (status == EFI_SUCCESS) {
		ST->ConOut->OutputString(ST->ConOut, L"First call to memory map succeeded (memory map is empty?)");
		hang();
	}
	if (status != EFI_BUFFER_TOO_SMALL) {
		error_check(L"GetMemoryMap 1");
	}
	UINTN correct_size = memory_map_size + (2 * descriptor_size);

	status = BS->AllocatePool(EfiLoaderData, correct_size, (void**) &memory_descriptors);
	if (status != EFI_SUCCESS) error_check(L"AllocatePool memory_map");
	
	memory_map_size = correct_size;
	status = BS->GetMemoryMap(&memory_map_size, memory_descriptors, &map_key, &descriptor_size, &version);
	error_check(L"GetMemoryMap 2");
	
	MemoryMap memory_map;
	memory_map.memory_descriptors = memory_descriptors;
	memory_map.memory_map_size = memory_map_size;
	memory_map.map_key = map_key;
	memory_map.descriptor_size = descriptor_size;

	return memory_map;
}

KernelEntryPoint load_kernel(uint64_t *kernel_start, uint64_t *kernel_end) {
	//Make sure the simple file protocol is supported
	EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
	EFI_GUID li_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	status = ST->BootServices->OpenProtocol(IH, &li_guid, (void**)&loaded_image, IH, 0, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	error_check(L"OpenProtocol LoadedImage");

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfs;
	EFI_GUID sfs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	status = ST->BootServices->OpenProtocol(loaded_image->DeviceHandle, &sfs_guid, (void**)&sfs, IH, 0, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
	error_check(L"OpenProtocol SimpleFileSystem");

	//Load kernel off disk into memory
	EFI_FILE_PROTOCOL *drive_root;
	status = sfs->OpenVolume(sfs, &drive_root);
	error_check(L"OpenVolume drive_root");

	EFI_FILE_PROTOCOL *kernel_file;
	status = drive_root->Open(drive_root, &kernel_file, kernel_path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (status == EFI_NOT_FOUND) {
		ST->ConOut->OutputString(ST->ConOut, L"Kernel could not be found at ");
		ST->ConOut->OutputString(ST->ConOut, kernel_path);
		ST->ConOut->OutputString(ST->ConOut, L"\r\n");
		hang();
	}
	error_check(L"Open kernel_file");

	EFI_FILE_INFO *file_info;
	EFI_GUID fi_guid = EFI_FILE_INFO_ID;

	//Keep allocating a bigger buffer until we succeed in getting the file info (8 tries)
	//We do this because we are not provided a method to find out how big of a buffer we need
	int allocated_buffer = 0;
	uint64_t buffer_size = 256;
	for (int i = 0; i < 8; i++) {
		status = BS->AllocatePool(EfiBootServicesData, buffer_size, (void**) &file_info);
		error_check(L"AllocatePool file_info");

		status = kernel_file->GetInfo(kernel_file, &fi_guid, &buffer_size, file_info);
	
		if (status == EFI_SUCCESS) {
			allocated_buffer = 1;
			break;
		}
		if (status != EFI_BUFFER_TOO_SMALL) error_check(L"GetInfo file_info");

		//Buffer wasn't big enough, double the size and reallocate it
		buffer_size *= 2;

		status = BS->FreePool((void*) file_info);
		error_check(L"LoopFreePool file_info");
	}

	if (!allocated_buffer) {
		ST->ConOut->OutputString(ST->ConOut, L"Failed to allocate file_info buffer");
		hang();
	}
	
	UINTN file_size = file_info->FileSize;
	status = BS->FreePool(file_info);
	error_check(L"FreePool file_info");

	UINTN pages_required = (file_size / 4096) + 1;
	uint8_t *kernel_buffer;
	status = BS->AllocatePages(AllocateAnyPages, EfiBootServicesData, pages_required, (EFI_PHYSICAL_ADDRESS*) &kernel_buffer);
	error_check(L"AllocatePages kernel_buffer");
	
	status = kernel_file->Read(kernel_file, &file_size, (void*) kernel_buffer);
	error_check(L"Read kernel_file");
	
	//Parse kernel file (ELF format)
	//NOTE: This is a 64 bit header, it will not correctly parse 32 bit ELF executable
 	ElfHeader *elf_header = (ElfHeader*) kernel_buffer;
	char *magic = elf_header->magic;
	if (magic[0] != 0x7f || magic[1] != 'E' || magic[2] != 'L' || magic[3] != 'F') {
		ST->ConOut->OutputString(ST->ConOut, L"Kernel is not a valid ELF file\r\n");
		hang();
	}

	if (elf_header->architecture != 2) {
		ST->ConOut->OutputString(ST->ConOut, L"Kernel is not 64 bit\r\n");
		hang();
	}

	if (elf_header->type != 2) {
		ST->ConOut->OutputString(ST->ConOut, L"Kernel must be compiled as executable (-static)\r\n");
		hang();
	}

	uint64_t lowest_address = -1;
	uint64_t highest_address = -1;
	for (int i = 0; i < elf_header->header_entry_count; i++) {
		ProgramHeader *program_header = (ProgramHeader*) ((uint64_t) kernel_buffer + (elf_header->header_table + (i * elf_header->header_entry_size)));
		if (program_header->segment_type == 1) {
			uint64_t memory_end = program_header->virtual_address + program_header->memory_segment_size;
			if (lowest_address == -1 || program_header->virtual_address < lowest_address) lowest_address = program_header->virtual_address;
			if (highest_address == -1 || memory_end > highest_address) highest_address = memory_end;
		}
	}

	*kernel_start = lowest_address;
	*kernel_end = highest_address;
	
	uint64_t program_size = highest_address - lowest_address;
	pages_required = (program_size / 4096) + 1;

	//Allocate memory for kernel
	EFI_PHYSICAL_ADDRESS kernel = lowest_address;
	status = BS->AllocatePages(AllocateAddress, EfiBootServicesData, pages_required, &kernel);
	error_check(L"AllocatePages kernel");
	
	//Copy kernel sections into newly allocated memory
	for (int i = 0; i < elf_header->header_entry_count; i++) {
		ProgramHeader *program_header = (ProgramHeader*) ((uint64_t) kernel_buffer + (elf_header->header_table + (i * elf_header->header_entry_size)));
		if (program_header->segment_type == 1) {
			for (int j = 0; j < program_header->file_segment_size; j++) {
				*((uint8_t*) program_header->virtual_address + j) = *(kernel_buffer + program_header->offset + j);
				//zero the rest?
			}
		}
	}

	uint64_t elf_entry_point = elf_header->entry_point;

	//Free original kernel buffer
	status = BS->FreePages((EFI_PHYSICAL_ADDRESS) kernel_buffer, pages_required);
	error_check(L"FreePages kernel_buffer");

	return (KernelEntryPoint) elf_entry_point;
}

GraphicsInfo initialize_graphics() {
	EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

	status = BS->LocateProtocol(&gop_guid, 0, (void**) &gop);
	error_check(L"LocateProtocol gop");

	GraphicsInfo graphics_info;
	graphics_info.address = (void*) gop->Mode->FrameBufferBase;
	graphics_info.buffer_size = gop->Mode->FrameBufferSize;
	graphics_info.width = gop->Mode->Info->HorizontalResolution;
	graphics_info.height = gop->Mode->Info->VerticalResolution;
	graphics_info.pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
	return graphics_info;
}

void* memcpy(void *destination, void *source, uint64_t size) {
	uint8_t *destination_bytes = (uint8_t*) destination;
	uint8_t *source_bytes = (uint8_t*) source;
	for (uint64_t i = 0; i < size; i++) *(destination_bytes + i) = *(source_bytes + i);
	return destination;
}

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *systab) {
	//InitializeLib(IH, systab);
	ST = systab;
	BS = ST->BootServices;
	IH = image_handle;

	//Clear screen before using it
	status = ST->ConOut->ClearScreen(ST->ConOut);
	error_check(L"ClearScreen");

	//Disable Watchdog timer to prevent system from rebooting after 5 minutes
	status = BS->SetWatchdogTimer(0, 0, 0, 0);
	error_check(L"ClearWatchdogTimer");

	uint64_t kernel_start;
	uint64_t kernel_end;
	KernelEntryPoint entry_point = load_kernel(&kernel_start, &kernel_end);
	MemoryMap memory_map = get_memory_map();
	GraphicsInfo graphics_info = initialize_graphics();
	if (graphics_info.width != graphics_info.pixels_per_scanline) {
		//I don't understand the different between these 2 metrics and I believe they will always be the same. Just in case they aren't, fail loudly.
		ST->ConOut->OutputString(ST->ConOut, L"Mismatch between width and pixels per scanline.");
		hang();
	}
	
	status = BS->ExitBootServices(IH, memory_map.map_key);
	error_check(L"ExitBootServices");

	KernelArguments kernel_arguments;
	kernel_arguments.memory_map = memory_map;
	kernel_arguments.graphics_info = graphics_info;
	kernel_arguments.kernel_start = kernel_start;
	kernel_arguments.kernel_end = kernel_end;

	asm("mov %0, %%rdi" : : "g" (&kernel_arguments));

	entry_point();
	
	ST->ConOut->ClearScreen(ST->ConOut);
	ST->ConOut->OutputString(ST->ConOut, L"Kernel exited, this should never happen.\r\n");
	hang();

	return EFI_SUCCESS;
}
