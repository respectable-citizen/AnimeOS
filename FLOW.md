BOOTLOADER:
* remove watchdog timer (done)
* read kernel file into buffer (done)
* allocate enough pages for kernel (done)
* write kernel into memory (done)
* free original kernel buffer (code written but commented out because it's buggy)
* get memory map (done)
* get GOP info
* exit boot services (done)
* call kernel main with required arguments (pointer to ACPI table, memory map, GOP info) (done)

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
