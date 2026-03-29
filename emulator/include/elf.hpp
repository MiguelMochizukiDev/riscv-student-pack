#pragma once
#include <cstdint>
#include <string>

class CPU;

class ELFLoader {
      public:
	bool load(const std::string &filename, CPU &cpu);

      private:
	static constexpr int EI_NIDENT = 16;

	struct Elf32_Ehdr {
		unsigned char e_ident[EI_NIDENT];
		uint16_t e_type;
		uint16_t e_machine;
		uint32_t e_version;
		uint32_t e_entry;
		uint32_t e_phoff;
		uint32_t e_shoff;
		uint32_t e_flags;
		uint16_t e_ehsize;
		uint16_t e_phentsize;
		uint16_t e_phnum;
		uint16_t e_shentsize;
		uint16_t e_shnum;
		uint16_t e_shstrndx;
	};

	struct Elf32_Phdr {
		uint32_t p_type;
		uint32_t p_offset;
		uint32_t p_vaddr;
		uint32_t p_paddr;
		uint32_t p_filesz;
		uint32_t p_memsz;
		uint32_t p_flags;
		uint32_t p_align;
	};

	struct Elf32_Shdr {
		uint32_t sh_name;
		uint32_t sh_type;
		uint32_t sh_flags;
		uint32_t sh_addr;
		uint32_t sh_offset;
		uint32_t sh_size;
		uint32_t sh_link;
		uint32_t sh_info;
		uint32_t sh_addralign;
		uint32_t sh_entsize;
	};

	struct Elf32_Sym {
		uint32_t st_name;
		uint32_t st_value;
		uint32_t st_size;
		uint8_t st_info;
		uint8_t st_other;
		uint16_t st_shndx;
	};

	static constexpr uint32_t PT_LOAD = 1;
	static constexpr uint32_t SHT_SYMTAB = 2;
	static constexpr uint32_t SHT_STRTAB = 3;

	static constexpr uint16_t ET_EXEC = 2;
	static constexpr uint16_t EM_RISCV = 0xF3;

	bool validate(const Elf32_Ehdr &ehdr);
	void loadSegments(std::ifstream &file, const Elf32_Ehdr &ehdr, CPU &cpu);
	void resolveGlobalPointer(std::ifstream &file, const Elf32_Ehdr &ehdr, CPU &cpu);
};