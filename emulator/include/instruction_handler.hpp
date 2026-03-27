/* instruction_handler.hpp */

#pragma once
#include <cstdint>
class CPU;

class InstructionHandler {
      public:
	virtual ~InstructionHandler() = default;

	virtual bool handle(CPU &cpu, uint32_t instr, uint32_t &next_pc) = 0;
};
