/* cpu.cpp */

#include "../include/cpu.hpp"
#include <cstdio>
#include <cstring>

/* ================= INIT ================= */

CPU::CPU(Memory &mem) : memory(mem) {
	init();
}

void CPU::init() {
	pc = 0;
	regs.fill(0);
	regs[2] = static_cast<uint32_t>(memory.getSize()) - 4;
	running = true;
}

/* ================= INTERFACE ================= */

bool CPU::isRunning() const { return running; }

uint32_t CPU::getPC() const { return pc; }

void CPU::setPC(uint32_t value) { pc = value; }

uint32_t CPU::getReg(int i) const { return regs[i]; }

void CPU::setReg(int i, uint32_t value) {
	if (i != 0)
		regs[i] = value;
}

void CPU::setRunning(bool state) { running = state; }

Memory &CPU::getMemory() { return memory; }

const Memory &CPU::getMemory() const { return memory; }

void CPU::writeMemory(uint32_t addr, const uint8_t *data, size_t size) {
	if (addr + size > memory.getSize()) {
		printf("Memory write out of bounds!\n");
		return;
	}
	memory.write(addr, data, size);
}

void CPU::zeroMemory(uint32_t addr, size_t size) {
	if (addr + size > memory.getSize()) {
		printf("Memory zero out of bounds!\n");
		return;
	}
	memory.zero(addr, size);
}

void CPU::dumpRegisters() const {
	for (int i = 0; i < 32; i++)
		printf("x%d: 0x%08X\n", i, regs[i]);
}

void CPU::registerHandler(std::unique_ptr<InstructionHandler> handler) {
	handlers.push_back(std::move(handler));
}

/* ================= CORE ================= */

void CPU::step() {
	uint32_t instr = fetch();
	decodeExecute(instr);
}

/* ================= FETCH ================= */

uint32_t CPU::fetch() {
	uint32_t instr = memory.load32(pc);
	printf("PC: 0x%08X | INSTR: 0x%08X\n", pc, instr);
	return instr;
}

/* ================= DECODE ================= */

void CPU::decodeExecute(uint32_t instr) {
	uint32_t next_pc = pc + 4;

	bool executed = false;
	for (auto &handler : handlers) {
		if (handler->handle(*this, instr, next_pc)) {
			executed = true;
			break;
		}
	}

	if (!executed) {
		printf("Unknown instruction 0x%08X at PC 0x%08X\n", instr, pc);
		setRunning(false);
	}

	commit(next_pc);
}

/* ================= COMMIT ================= */

void CPU::commit(uint32_t next_pc) {
	pc = next_pc;
	regs[0] = 0;
}