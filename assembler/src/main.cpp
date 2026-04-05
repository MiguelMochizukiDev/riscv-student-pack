#include "../include/assembler.hpp"
#include "../include/mnemonic_handler.hpp"
#include <exception>
#include <iostream>
#include <memory>

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <input.s> <output.elf>\n";
		return 1;
	}

	try {
		Assembler assembler;
		assembler.registerHandler(std::make_unique<RV32IMnemonicHandler>());
		assembler.assembleFile(argv[1], argv[2]);
		std::cout << "Assembled " << argv[1] << " -> " << argv[2] << "\n";
		return 0;
	} catch (const std::exception &ex) {
		std::cerr << "Assembler error: " << ex.what() << "\n";
		return 1;
	}
}
