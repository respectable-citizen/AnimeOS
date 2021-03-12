BOOTLOADER:
* remove watchdog timer (done)
* read kernel file into buffer (done)
* allocate enough pages for kernel (done)
* write kernel into memory (done)
* free original kernel buffer (done)
* get memory map (done)
* get GOP info (done)
* exit boot services (done)
* call kernel main with required arguments (pointer to ACPI table, memory map, GOP info) (done)

KERNEL:
* disable interrupts until we are able to handle them (done)
- check if CPU supports required features
- setup text rendering and import font files (done)
- init ACPI
* init GDT (done)
* init TSS
* init APIC
* init interrupt handling (done)
* init exception handlers (done)
* re-enable interrupts (done)
* initialise virtual memory using memory map (done)
* init timer
* init keyboard controller
* init random
* start task scheduler
