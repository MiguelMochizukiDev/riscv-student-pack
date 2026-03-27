/* cpu.hpp */

#ifndef CPU_HPP
#define CPU_HPP

#include "instruction_handler.hpp"
#include "memory.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

class CPU {
      private:
	uint32_t pc;
	std::array<uint32_t, 32> regs;
	Memory &memory;
	bool running;
	std::vector<InstructionHandler *> handlers;

	uint32_t fetch();
	void decodeExecute(uint32_t instr);

	void commit(uint32_t next_pc);

      public:
	CPU(Memory &mem);

	void init();
	void step();

	uint32_t getPC() const;
	void setPC(uint32_t value);

	uint32_t getReg(int i) const;
	void setReg(int i, uint32_t value);

	bool isRunning() const;
	void setRunning(bool state);

	Memory &getMemory();
	const Memory &getMemory() const;

	void writeMemory(uint32_t addr, const uint8_t *data, size_t size);
	void zeroMemory(uint32_t addr, size_t size);

	void dumpRegisters() const;

	void registerHandler(InstructionHandler *h);
};

#endif