/* rv32i_handler.hpp */

#pragma once
#include "instruction_handler.hpp"

class RV32IHandler : public InstructionHandler {
      public:
	bool handle(CPU &cpu, uint32_t instr, uint32_t &next_pc) override;

      private:
	static int32_t signExtend(uint32_t value, int bits);

	bool handleRType(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleIType(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleLoad(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleStore(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleBranch(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleJAL(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleJALR(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleLUI(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleAUIPC(CPU &cpu, uint32_t instr, uint32_t &next_pc);
	bool handleSystem(CPU &cpu, uint32_t instr, uint32_t &next_pc);
};
