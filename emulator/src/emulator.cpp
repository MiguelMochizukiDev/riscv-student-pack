/* emulator.cpp */

#include "../include/emulator.hpp"
#include <iostream>

Emulator::Emulator() : memory(), cpu(memory) {}

void Emulator::registerHandler(std::unique_ptr<InstructionHandler> handler) {
	cpu.registerHandler(std::move(handler));
}

bool Emulator::loadELF(const std::string &path) {
	return loader.load(path, cpu);
}

void Emulator::run() {
	while (cpu.isRunning())
		cpu.step();
}

void Emulator::step() {
	if (cpu.isRunning())
		cpu.step();
}

const CPU &Emulator::getCPU() const {
	return cpu;
}

void Emulator::dumpRegisters() const {
	cpu.dumpRegisters();
}