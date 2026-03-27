/* elf.cpp */

#include "../include/elf.hpp"
#include "../include/cpu.hpp"

#include <fstream>
#include <iostream>
#include <vector>

bool ELFLoader::validate(const Elf32_Ehdr &ehdr) {
	return (ehdr.e_ident[0] == 0x7F && ehdr.e_ident[1] == 'E' &&
		ehdr.e_ident[2] == 'L' && ehdr.e_ident[3] == 'F' &&
		ehdr.e_ident[4] == 1);
}

bool ELFLoader::load(const std::string &filename, CPU &cpu) {
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		std::cerr << "Failed to open file\n";
		return false;
	}

	Elf32_Ehdr ehdr;
	file.read(reinterpret_cast<char *>(&ehdr), sizeof(ehdr));

	if (!validate(ehdr)) {
		std::cerr << "Invalid ELF file\n";
		return false;
	}

	file.seekg(ehdr.e_phoff);

	for (int i = 0; i < ehdr.e_phnum; i++) {
		Elf32_Phdr phdr;
		file.read(reinterpret_cast<char *>(&phdr), sizeof(phdr));

		if (phdr.p_type != PT_LOAD)
			continue;

		std::vector<uint8_t> buffer(phdr.p_filesz);

		file.seekg(phdr.p_offset);
		file.read(reinterpret_cast<char *>(buffer.data()), phdr.p_filesz);

		cpu.writeMemory(phdr.p_vaddr, buffer.data(), phdr.p_filesz);

		if (phdr.p_memsz > phdr.p_filesz) {
			cpu.zeroMemory(phdr.p_vaddr + phdr.p_filesz,
				       phdr.p_memsz - phdr.p_filesz);
		}

		file.seekg(ehdr.e_phoff + (i + 1) * sizeof(Elf32_Phdr));
	}

	cpu.setPC(ehdr.e_entry);

	return true;
}