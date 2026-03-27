/* emulator.hpp */

#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "cpu.hpp"
#include "elf.hpp"
#include "memory.hpp"
#include <memory>
#include <string>

class Emulator {
      private:
	Memory memory;
	CPU cpu;
	ELFLoader loader;

      public:
	Emulator();

	/* Registers an instruction handler, transferring ownership to the CPU. */
	void registerHandler(std::unique_ptr<InstructionHandler> handler);

	/* Loads an ELF binary into memory and sets the entry point. */
	bool loadELF(const std::string &path);

	/* Runs until the CPU halts. */
	void run();

	/* Executes a single instruction. */
	void step();

	const CPU &getCPU() const;

	void dumpRegisters() const;
};

#endif