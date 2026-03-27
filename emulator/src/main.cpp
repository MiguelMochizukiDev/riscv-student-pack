/* main.cpp*/

#include "../include/cpu.hpp"
#include "../include/elf.hpp"
#include "../include/rv32i_handler.hpp"
#include <iostream>

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <elf>\n";
		return 1;
	}

	Memory memory;
	CPU cpu(memory);
	ELFLoader loader;

	RV32IHandler rv32i;
	cpu.registerHandler(&rv32i);

	if (!loader.load(argv[1], cpu))
		return 1;

	std::cout << "Program loaded. Entry point: 0x" << std::hex << cpu.getPC()
		  << "\n";

	while (cpu.isRunning()) {
		cpu.step();
	}

	std::cout << "\nFinal registers:\n";
	cpu.dumpRegisters();
	return 0;
}