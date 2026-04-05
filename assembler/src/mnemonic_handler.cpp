#include "../include/mnemonic_handler.hpp"
#include <stdexcept>

static uint32_t encodeRType(uint32_t funct7, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode) {
	return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}

static uint32_t encodeIType(int32_t imm, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode) {
	uint32_t uimm = static_cast<uint32_t>(imm) & 0xFFF;
	return (uimm << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode;
}

static uint32_t encodeSType(int32_t imm, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode) {
	uint32_t uimm = static_cast<uint32_t>(imm) & 0xFFF;
	uint32_t immHi = (uimm >> 5) & 0x7F;
	uint32_t immLo = uimm & 0x1F;
	return (immHi << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (immLo << 7) | opcode;
}

static uint32_t encodeBType(int32_t imm, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode) {
	uint32_t uimm = static_cast<uint32_t>(imm) & 0x1FFF;
	uint32_t bit12 = (uimm >> 12) & 0x1;
	uint32_t bit11 = (uimm >> 11) & 0x1;
	uint32_t bits10to5 = (uimm >> 5) & 0x3F;
	uint32_t bits4to1 = (uimm >> 1) & 0xF;
	return (bit12 << 31) | (bits10to5 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (bits4to1 << 8) | (bit11 << 7) |
	       opcode;
}

static uint32_t encodeUType(int32_t imm, uint32_t rd, uint32_t opcode) {
	uint32_t uimm = static_cast<uint32_t>(imm) & 0xFFFFF000;
	return uimm | (rd << 7) | opcode;
}

static uint32_t encodeJType(int32_t imm, uint32_t rd, uint32_t opcode) {
	uint32_t uimm = static_cast<uint32_t>(imm) & 0x1FFFFF;
	uint32_t bit20 = (uimm >> 20) & 0x1;
	uint32_t bits10to1 = (uimm >> 1) & 0x3FF;
	uint32_t bit11 = (uimm >> 11) & 0x1;
	uint32_t bits19to12 = (uimm >> 12) & 0xFF;
	return (bit20 << 31) | (bits10to1 << 21) | (bit11 << 20) | (bits19to12 << 12) | (rd << 7) | opcode;
}

static void requireOperandCount(const InstructionNode &instruction, size_t expected) {
	if (instruction.operands.size() != expected) {
		throw std::runtime_error("Instruction '" + instruction.mnemonic + "' expects " + std::to_string(expected) + " operands (line " +
					 std::to_string(instruction.line) + ")");
	}
}

static int operandAsRegister(const Operand &operand, const std::string &mnemonic, int line) {
	if (operand.kind != OperandKind::Register)
		throw std::runtime_error("Instruction '" + mnemonic + "' expects register operand at line " + std::to_string(line));
	return static_cast<int>(std::get<int32_t>(operand.value));
}

static int32_t operandAsImmediate(const Operand &operand, const std::string &mnemonic, int line, const ISymbolResolver &resolver) {
	if (operand.kind == OperandKind::Immediate)
		return std::get<int32_t>(operand.value);
	if (operand.kind == OperandKind::Symbol) {
		uint32_t value = 0;
		if (!resolver.tryResolve(std::get<std::string>(operand.value), value))
			throw std::runtime_error("Undefined symbol in '" + mnemonic + "' at line " + std::to_string(line));
		return static_cast<int32_t>(value);
	}
	throw std::runtime_error("Instruction '" + mnemonic + "' expects immediate/symbol operand at line " + std::to_string(line));
}

static void ensureSignedRange(int32_t value, int bits, const std::string &message) {
	const int32_t min = -(1 << (bits - 1));
	const int32_t max = (1 << (bits - 1)) - 1;
	if (value < min || value > max)
		throw std::runtime_error(message);
}

static void ensureUnsignedRange(uint32_t value, int bits, const std::string &message) {
	const uint32_t max = (1u << bits) - 1u;
	if (value > max)
		throw std::runtime_error(message);
}

static MemoryOperand operandAsMemory(const Operand &operand, const std::string &mnemonic, int line) {
	if (operand.kind != OperandKind::Memory)
		throw std::runtime_error("Instruction '" + mnemonic + "' expects memory operand at line " + std::to_string(line));
	return std::get<MemoryOperand>(operand.value);
}

bool RV32IMnemonicHandler::handle(const InstructionNode &instruction, const EncodeContext &context, uint32_t &encodedInstruction) const {
	const std::string &mnemonic = instruction.mnemonic;

	if (mnemonic == "nop") {
		requireOperandCount(instruction, 0);
		encodedInstruction = encodeIType(0, 0, 0x0, 0, 0x13);
		return true;
	}

	if (mnemonic == "add") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x0, rd, 0x33);
		return true;
	}

	if (mnemonic == "sub") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x20, rs2, rs1, 0x0, rd, 0x33);
		return true;
	}

	if (mnemonic == "sll") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x1, rd, 0x33);
		return true;
	}

	if (mnemonic == "slt") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x2, rd, 0x33);
		return true;
	}

	if (mnemonic == "sltu") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x3, rd, 0x33);
		return true;
	}

	if (mnemonic == "xor") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x4, rd, 0x33);
		return true;
	}

	if (mnemonic == "srl") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x5, rd, 0x33);
		return true;
	}

	if (mnemonic == "sra") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x20, rs2, rs1, 0x5, rd, 0x33);
		return true;
	}

	if (mnemonic == "or") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x6, rd, 0x33);
		return true;
	}

	if (mnemonic == "and") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[2], mnemonic, instruction.line);
		encodedInstruction = encodeRType(0x00, rs2, rs1, 0x7, rd, 0x33);
		return true;
	}

	if (mnemonic == "addi") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		ensureSignedRange(imm, 12, "addi immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(imm, rs1, 0x0, rd, 0x13);
		return true;
	}

	if (mnemonic == "slti") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		ensureSignedRange(imm, 12, "slti immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(imm, rs1, 0x2, rd, 0x13);
		return true;
	}

	if (mnemonic == "sltiu") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		ensureSignedRange(imm, 12, "sltiu immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(imm, rs1, 0x3, rd, 0x13);
		return true;
	}

	if (mnemonic == "xori") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		ensureSignedRange(imm, 12, "xori immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(imm, rs1, 0x4, rd, 0x13);
		return true;
	}

	if (mnemonic == "ori") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		ensureSignedRange(imm, 12, "ori immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(imm, rs1, 0x6, rd, 0x13);
		return true;
	}

	if (mnemonic == "andi") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		ensureSignedRange(imm, 12, "andi immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(imm, rs1, 0x7, rd, 0x13);
		return true;
	}

	if (mnemonic == "slli") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t shamt = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		if (shamt < 0)
			throw std::runtime_error("slli shamt must be non-negative at line " + std::to_string(instruction.line));
		ensureUnsignedRange(static_cast<uint32_t>(shamt), 5, "slli shamt out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(static_cast<int32_t>(shamt), rs1, 0x1, rd, 0x13);
		return true;
	}

	if (mnemonic == "srli") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t shamt = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		if (shamt < 0)
			throw std::runtime_error("srli shamt must be non-negative at line " + std::to_string(instruction.line));
		ensureUnsignedRange(static_cast<uint32_t>(shamt), 5, "srli shamt out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(static_cast<int32_t>(shamt), rs1, 0x5, rd, 0x13);
		return true;
	}

	if (mnemonic == "srai") {
		requireOperandCount(instruction, 3);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t shamt = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		if (shamt < 0)
			throw std::runtime_error("srai shamt must be non-negative at line " + std::to_string(instruction.line));
		ensureUnsignedRange(static_cast<uint32_t>(shamt), 5, "srai shamt out of range at line " + std::to_string(instruction.line));
		int32_t imm = (0x20 << 5) | static_cast<int32_t>(shamt);
		encodedInstruction = encodeIType(imm, rs1, 0x5, rd, 0x13);
		return true;
	}

	if (mnemonic == "lui") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[1], mnemonic, instruction.line, context.resolver);
		encodedInstruction = encodeUType(imm, rd, 0x37);
		return true;
	}

	if (mnemonic == "jal") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int32_t target = operandAsImmediate(instruction.operands[1], mnemonic, instruction.line, context.resolver);
		int32_t offset = target - static_cast<int32_t>(context.currentAddress);
		if ((offset & 1) != 0)
			throw std::runtime_error("jal target must be 2-byte aligned at line " + std::to_string(instruction.line));
		ensureSignedRange(offset, 21, "jal offset out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeJType(offset, rd, 0x6F);
		return true;
	}

	if (mnemonic == "jalr") {
		if (instruction.operands.size() != 2 && instruction.operands.size() != 3)
			throw std::runtime_error("Instruction 'jalr' expects 2 or 3 operands (line " + std::to_string(instruction.line) + ")");

		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs1 = 0;
		int32_t imm = 0;
		if (instruction.operands.size() == 2) {
			MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
			rs1 = mem.baseRegister;
			imm = mem.offset;
		} else {
			rs1 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
			imm = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		}
		ensureSignedRange(imm, 12, "jalr immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(imm, rs1, 0x0, rd, 0x67);
		return true;
	}

	if (mnemonic == "lb") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "lb immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(mem.offset, mem.baseRegister, 0x0, rd, 0x03);
		return true;
	}

	if (mnemonic == "lh") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "lh immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(mem.offset, mem.baseRegister, 0x1, rd, 0x03);
		return true;
	}

	if (mnemonic == "lw") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "lw immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(mem.offset, mem.baseRegister, 0x2, rd, 0x03);
		return true;
	}

	if (mnemonic == "lbu") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "lbu immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(mem.offset, mem.baseRegister, 0x4, rd, 0x03);
		return true;
	}

	if (mnemonic == "lhu") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "lhu immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeIType(mem.offset, mem.baseRegister, 0x5, rd, 0x03);
		return true;
	}

	if (mnemonic == "sb") {
		requireOperandCount(instruction, 2);
		int rs2 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "sb immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeSType(mem.offset, rs2, mem.baseRegister, 0x0, 0x23);
		return true;
	}

	if (mnemonic == "sh") {
		requireOperandCount(instruction, 2);
		int rs2 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "sh immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeSType(mem.offset, rs2, mem.baseRegister, 0x1, 0x23);
		return true;
	}

	if (mnemonic == "sw") {
		requireOperandCount(instruction, 2);
		int rs2 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		MemoryOperand mem = operandAsMemory(instruction.operands[1], mnemonic, instruction.line);
		ensureSignedRange(mem.offset, 12, "sw immediate out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeSType(mem.offset, rs2, mem.baseRegister, 0x2, 0x23);
		return true;
	}

	if (mnemonic == "beq") {
		requireOperandCount(instruction, 3);
		int rs1 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t target = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		int32_t offset = target - static_cast<int32_t>(context.currentAddress);
		if ((offset & 1) != 0)
			throw std::runtime_error("branch target must be 2-byte aligned at line " + std::to_string(instruction.line));
		ensureSignedRange(offset, 13, "branch offset out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeBType(offset, rs2, rs1, 0x0, 0x63);
		return true;
	}

	if (mnemonic == "bne") {
		requireOperandCount(instruction, 3);
		int rs1 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t target = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		int32_t offset = target - static_cast<int32_t>(context.currentAddress);
		if ((offset & 1) != 0)
			throw std::runtime_error("branch target must be 2-byte aligned at line " + std::to_string(instruction.line));
		ensureSignedRange(offset, 13, "branch offset out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeBType(offset, rs2, rs1, 0x1, 0x63);
		return true;
	}

	if (mnemonic == "blt") {
		requireOperandCount(instruction, 3);
		int rs1 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t target = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		int32_t offset = target - static_cast<int32_t>(context.currentAddress);
		if ((offset & 1) != 0)
			throw std::runtime_error("branch target must be 2-byte aligned at line " + std::to_string(instruction.line));
		ensureSignedRange(offset, 13, "branch offset out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeBType(offset, rs2, rs1, 0x4, 0x63);
		return true;
	}

	if (mnemonic == "bge") {
		requireOperandCount(instruction, 3);
		int rs1 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t target = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		int32_t offset = target - static_cast<int32_t>(context.currentAddress);
		if ((offset & 1) != 0)
			throw std::runtime_error("branch target must be 2-byte aligned at line " + std::to_string(instruction.line));
		ensureSignedRange(offset, 13, "branch offset out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeBType(offset, rs2, rs1, 0x5, 0x63);
		return true;
	}

	if (mnemonic == "bltu") {
		requireOperandCount(instruction, 3);
		int rs1 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t target = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		int32_t offset = target - static_cast<int32_t>(context.currentAddress);
		if ((offset & 1) != 0)
			throw std::runtime_error("branch target must be 2-byte aligned at line " + std::to_string(instruction.line));
		ensureSignedRange(offset, 13, "branch offset out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeBType(offset, rs2, rs1, 0x6, 0x63);
		return true;
	}

	if (mnemonic == "bgeu") {
		requireOperandCount(instruction, 3);
		int rs1 = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int rs2 = operandAsRegister(instruction.operands[1], mnemonic, instruction.line);
		int32_t target = operandAsImmediate(instruction.operands[2], mnemonic, instruction.line, context.resolver);
		int32_t offset = target - static_cast<int32_t>(context.currentAddress);
		if ((offset & 1) != 0)
			throw std::runtime_error("branch target must be 2-byte aligned at line " + std::to_string(instruction.line));
		ensureSignedRange(offset, 13, "branch offset out of range at line " + std::to_string(instruction.line));
		encodedInstruction = encodeBType(offset, rs2, rs1, 0x7, 0x63);
		return true;
	}

	if (mnemonic == "auipc") {
		requireOperandCount(instruction, 2);
		int rd = operandAsRegister(instruction.operands[0], mnemonic, instruction.line);
		int32_t imm = operandAsImmediate(instruction.operands[1], mnemonic, instruction.line, context.resolver);
		encodedInstruction = encodeUType(imm, rd, 0x17);
		return true;
	}

	if (mnemonic == "ecall") {
		requireOperandCount(instruction, 0);
		encodedInstruction = 0x00000073;
		return true;
	}

	if (mnemonic == "ebreak") {
		requireOperandCount(instruction, 0);
		encodedInstruction = 0x00100073;
		return true;
	}

	return false;
}
