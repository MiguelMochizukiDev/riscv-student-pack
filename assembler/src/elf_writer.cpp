#include "../include/elf_writer.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>

#pragma pack(push, 1)
struct Elf32_Ehdr {
	std::array<unsigned char, 16> e_ident{};
	uint16_t e_type = 0;
	uint16_t e_machine = 0;
	uint32_t e_version = 0;
	uint32_t e_entry = 0;
	uint32_t e_phoff = 0;
	uint32_t e_shoff = 0;
	uint32_t e_flags = 0;
	uint16_t e_ehsize = 0;
	uint16_t e_phentsize = 0;
	uint16_t e_phnum = 0;
	uint16_t e_shentsize = 0;
	uint16_t e_shnum = 0;
	uint16_t e_shstrndx = 0;
};

struct Elf32_Phdr {
	uint32_t p_type = 0;
	uint32_t p_offset = 0;
	uint32_t p_vaddr = 0;
	uint32_t p_paddr = 0;
	uint32_t p_filesz = 0;
	uint32_t p_memsz = 0;
	uint32_t p_flags = 0;
	uint32_t p_align = 0;
};
#pragma pack(pop)

static constexpr uint16_t ET_EXEC = 2;
static constexpr uint16_t EM_RISCV = 0xF3;
static constexpr uint32_t EV_CURRENT = 1;
static constexpr uint32_t PT_LOAD = 1;
static constexpr uint32_t PF_X = 1;
static constexpr uint32_t PF_R = 4;

void ELFWriter::writeExecutable(const std::string &outputPath, const AssemblyImage &image) const {
	std::ofstream out(outputPath, std::ios::binary);
	if (!out)
		throw std::runtime_error("Failed to open output ELF: " + outputPath);

	const uint32_t headerSize = static_cast<uint32_t>(sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr));
	const uint32_t segmentOffset = 0x100;
	if (segmentOffset < headerSize)
		throw std::runtime_error("Internal ELF layout error: segment offset overlaps headers");

	Elf32_Ehdr ehdr;
	ehdr.e_ident = {0x7F, 'E', 'L', 'F', 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	ehdr.e_type = ET_EXEC;
	ehdr.e_machine = EM_RISCV;
	ehdr.e_version = EV_CURRENT;
	ehdr.e_entry = image.entryPoint;
	ehdr.e_phoff = sizeof(Elf32_Ehdr);
	ehdr.e_shoff = 0;
	ehdr.e_flags = 0;
	ehdr.e_ehsize = sizeof(Elf32_Ehdr);
	ehdr.e_phentsize = sizeof(Elf32_Phdr);
	ehdr.e_phnum = 1;
	ehdr.e_shentsize = 0;
	ehdr.e_shnum = 0;
	ehdr.e_shstrndx = 0;

	Elf32_Phdr phdr;
	phdr.p_type = PT_LOAD;
	phdr.p_offset = segmentOffset;
	phdr.p_vaddr = image.baseAddress;
	phdr.p_paddr = image.baseAddress;
	phdr.p_filesz = static_cast<uint32_t>(image.text.size());
	phdr.p_memsz = static_cast<uint32_t>(image.text.size());
	phdr.p_flags = PF_R | PF_X;
	phdr.p_align = 0x1000;

	out.write(reinterpret_cast<const char *>(&ehdr), sizeof(ehdr));
	out.write(reinterpret_cast<const char *>(&phdr), sizeof(phdr));

	const uint32_t writtenHeaders = static_cast<uint32_t>(out.tellp());
	for (uint32_t i = writtenHeaders; i < segmentOffset; ++i)
		out.put('\0');

	if (!image.text.empty())
		out.write(reinterpret_cast<const char *>(image.text.data()), static_cast<std::streamsize>(image.text.size()));

	if (!out)
		throw std::runtime_error("Failed while writing ELF contents");
}
