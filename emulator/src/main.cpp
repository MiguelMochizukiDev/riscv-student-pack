/* main.cpp */

#include "../include/emulator.hpp"
#include "../include/rv32i_handler.hpp"
#include <iostream>
#include <memory>

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <elf>\n";
		return 1;
	}

	Emulator emu;
	emu.registerHandler(std::make_unique<RV32IHandler>());

	if (!emu.loadELF(argv[1]))
		return 1;

	std::cout << "Program loaded. Entry point: 0x" << std::hex
		  << emu.getCPU().getPC() << "\n";

	emu.run();

	std::cout << "\nFinal registers:\n";
	emu.dumpRegisters();
	return 0;
}