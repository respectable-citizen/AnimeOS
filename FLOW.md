BOOTLOADER:
* remove watchdog timer
* read kernel file into buffer
* allocate enough pages for kernel
* write kernel into memory
* free original kernel buffer
- switch into graphics mode
* get memory map
* exit boot services
* call kernel main with required arguments (pointer to ACPI table, memory map, pointer to graphics protocol instance)

KERNEL:
* disable interrupts until we are able to handle them
- check if CPU supports required features
- setup text rendering and import font files
- init ACPI
* init GDT (includes TSS)
* init APIC
* init interrupt handling
* init exception handlers
* re-enable interrupts
* initialise virtual memory using memory map
* init timer
* init keyboard controller
* init random
* start task scheduler
