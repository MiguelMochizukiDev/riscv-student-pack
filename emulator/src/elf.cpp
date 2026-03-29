#include "../include/elf.hpp"
#include "../include/cpu.hpp"
#include <fstream>
#include <iostream>
#include <vector>

bool ELFLoader::validate(const Elf32_Ehdr &ehdr) {
	if (ehdr.e_ident[0] != 0x7F || ehdr.e_ident[1] != 'E' ||
	    ehdr.e_ident[2] != 'L' || ehdr.e_ident[3] != 'F') {
		std::cerr << "ELFLoader: invalid magic number\n";
		return false;
	}
	if (ehdr.e_ident[4] != 1) {
		std::cerr << "ELFLoader: not a 32-bit ELF\n";
		return false;
	}
	if (ehdr.e_ident[5] != 1) {
		std::cerr << "ELFLoader: not a little-endian ELF\n";
		return false;
	}
	if (ehdr.e_type != ET_EXEC) {
		std::cerr << "ELFLoader: not an executable ELF\n";
		return false;
	}
	if (ehdr.e_machine != EM_RISCV) {
		std::cerr << "ELFLoader: not a RISC-V ELF\n";
		return false;
	}
	return true;
}

void ELFLoader::loadSegments(std::ifstream &file, const Elf32_Ehdr &ehdr, CPU &cpu) {
	for (int i = 0; i < ehdr.e_phnum; i++) {
		file.seekg(ehdr.e_phoff + i * sizeof(Elf32_Phdr));

		Elf32_Phdr phdr;
		file.read(reinterpret_cast<char *>(&phdr), sizeof(phdr));

		if (phdr.p_type != PT_LOAD)
			continue;

		std::vector<uint8_t> buffer(phdr.p_filesz);
		file.seekg(phdr.p_offset);
		file.read(reinterpret_cast<char *>(buffer.data()), phdr.p_filesz);
		cpu.writeMemory(phdr.p_vaddr, buffer.data(), phdr.p_filesz);

		if (phdr.p_memsz > phdr.p_filesz)
			cpu.zeroMemory(phdr.p_vaddr + phdr.p_filesz, phdr.p_memsz - phdr.p_filesz);
	}
}

void ELFLoader::resolveGlobalPointer(std::ifstream &file, const Elf32_Ehdr &ehdr, CPU &cpu) {
	if (ehdr.e_shoff == 0)
		return;

	std::vector<Elf32_Shdr> shdrs(ehdr.e_shnum);
	file.seekg(ehdr.e_shoff);
	for (auto &shdr : shdrs)
		file.read(reinterpret_cast<char *>(&shdr), sizeof(shdr));

	const Elf32_Shdr *symtab_hdr = nullptr;
	for (const auto &shdr : shdrs) {
		if (shdr.sh_type == SHT_SYMTAB) {
			symtab_hdr = &shdr;
			break;
		}
	}
	if (!symtab_hdr)
		return;

	const Elf32_Shdr &strtab_hdr = shdrs[symtab_hdr->sh_link];

	std::vector<char> strtab(strtab_hdr.sh_size);
	file.seekg(strtab_hdr.sh_offset);
	file.read(strtab.data(), strtab_hdr.sh_size);

	size_t sym_count = symtab_hdr->sh_size / sizeof(Elf32_Sym);
	file.seekg(symtab_hdr->sh_offset);

	for (size_t i = 0; i < sym_count; i++) {
		Elf32_Sym sym;
		file.read(reinterpret_cast<char *>(&sym), sizeof(sym));

		const char *name = strtab.data() + sym.st_name;
		if (std::string(name) == "__global_pointer$") {
			cpu.setReg(3, sym.st_value); // gp = x3
			std::printf("ELFLoader: gp = 0x%08X\n", sym.st_value);
			return;
		}
	}
}

bool ELFLoader::load(const std::string &filename, CPU &cpu) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "ELFLoader: failed to open '" << filename << "'\n";
		return false;
	}

	Elf32_Ehdr ehdr;
	file.read(reinterpret_cast<char *>(&ehdr), sizeof(ehdr));

	if (!validate(ehdr))
		return false;

	loadSegments(file, ehdr, cpu);
	resolveGlobalPointer(file, ehdr, cpu);
	cpu.setPC(ehdr.e_entry);

	std::printf("ELFLoader: loaded '%s', entry = 0x%08X\n", filename.c_str(), ehdr.e_entry);
	return true;
}