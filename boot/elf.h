typedef struct {
	char magic[4];
	uint8_t architecture;
	uint8_t endianness;
	uint8_t header_version;
	uint8_t abi;
	uint64_t padding;
	uint16_t type;
	uint16_t instruction_set;
	uint32_t elf_version;
	uint64_t entry_point;
	uint64_t header_table;
	uint64_t section_table;
	uint32_t flags;
	uint16_t header_size;
	uint16_t header_entry_size;
	uint16_t header_entry_count;
	uint16_t section_entry_size;
	uint16_t section_entry_count;
	uint16_t section_table_index;
} ElfHeader;

typedef struct {
	uint32_t segment_type;
	uint32_t flags;
	uint64_t offset;
	uint64_t virtual_address;
	uint64_t padding;
	uint64_t file_segment_size;
	uint64_t memory_segment_size;
	uint64_t alignment;
} ProgramHeader;


