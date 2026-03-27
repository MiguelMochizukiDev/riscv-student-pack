/* elf.hpp */

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

	static constexpr uint32_t PT_LOAD = 1;

	bool validate(const Elf32_Ehdr &ehdr);
};