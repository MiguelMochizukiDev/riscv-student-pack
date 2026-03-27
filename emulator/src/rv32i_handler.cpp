/* rv32i_handler.cpp */

#include "../include/rv32i_handler.hpp"
#include "../include/cpu.hpp"
#include <cstdint>

int32_t RV32IHandler::signExtend(uint32_t value, int bits) {
	int32_t shift = 32 - bits;
	return (int32_t)(value << shift) >> shift;
}

bool RV32IHandler::handle(CPU &cpu, uint32_t instr, uint32_t &next_pc) {
	uint32_t opcode = instr & 0x7F;

	switch (opcode) {
	case 0x33:
		return handleRType(cpu, instr, next_pc);
	case 0x13:
		return handleIType(cpu, instr, next_pc);
	case 0x03:
		return handleLoad(cpu, instr, next_pc);
	case 0x23:
		return handleStore(cpu, instr, next_pc);
	case 0x63:
		return handleBranch(cpu, instr, next_pc);
	case 0x6F:
		return handleJAL(cpu, instr, next_pc);
	case 0x67:
		return handleJALR(cpu, instr, next_pc);
	case 0x37:
		return handleLUI(cpu, instr, next_pc);
	case 0x17:
		return handleAUIPC(cpu, instr, next_pc);
	case 0x73:
		return handleSystem(cpu, instr, next_pc);
	default:
		return false;
	}
}

/* ===== R-TYPE ===== */
bool RV32IHandler::handleRType(CPU &cpu, uint32_t instr, uint32_t &) {
	uint32_t rd = (instr >> 7) & 0x1F;
	uint32_t funct3 = (instr >> 12) & 0x7;
	uint32_t rs1 = (instr >> 15) & 0x1F;
	uint32_t rs2 = (instr >> 20) & 0x1F;
	uint32_t funct7 = (instr >> 25) & 0x7F;

	switch (funct3) {
	case 0x0:
		cpu.setReg(rd, (funct7 == 0x20) ? cpu.getReg(rs1) - cpu.getReg(rs2)
						: cpu.getReg(rs1) + cpu.getReg(rs2));
		break;
	case 0x1:
		cpu.setReg(rd, cpu.getReg(rs1) << (cpu.getReg(rs2) & 0x1F));
		break;
	case 0x2:
		cpu.setReg(rd,
			   (int32_t(cpu.getReg(rs1)) < int32_t(cpu.getReg(rs2))) ? 1 : 0);
		break;
	case 0x3:
		cpu.setReg(rd, (cpu.getReg(rs1) < cpu.getReg(rs2)) ? 1 : 0);
		break;
	case 0x4:
		cpu.setReg(rd, cpu.getReg(rs1) ^ cpu.getReg(rs2));
		break;
	case 0x5:
		cpu.setReg(rd, (funct7 == 0x20)
				   ? int32_t(cpu.getReg(rs1)) >> (cpu.getReg(rs2) & 0x1F)
				   : cpu.getReg(rs1) >> (cpu.getReg(rs2) & 0x1F));
		break;
	case 0x6:
		cpu.setReg(rd, cpu.getReg(rs1) | cpu.getReg(rs2));
		break;
	case 0x7:
		cpu.setReg(rd, cpu.getReg(rs1) & cpu.getReg(rs2));
		break;
	}
	return true;
}

/* ===== I-TYPE ===== */
bool RV32IHandler::handleIType(CPU &cpu, uint32_t instr, uint32_t &) {
	uint32_t rd = (instr >> 7) & 0x1F;
	uint32_t funct3 = (instr >> 12) & 0x7;
	uint32_t rs1 = (instr >> 15) & 0x1F;
	int32_t imm = signExtend(instr >> 20, 12);

	switch (funct3) {
	case 0x0:
		cpu.setReg(rd, cpu.getReg(rs1) + imm);
		break;
	case 0x2:
		cpu.setReg(rd, (int32_t(cpu.getReg(rs1)) < imm) ? 1 : 0);
		break;
	case 0x3:
		cpu.setReg(rd, (cpu.getReg(rs1) < uint32_t(imm)) ? 1 : 0);
		break;
	case 0x4:
		cpu.setReg(rd, cpu.getReg(rs1) ^ imm);
		break;
	case 0x6:
		cpu.setReg(rd, cpu.getReg(rs1) | imm);
		break;
	case 0x7:
		cpu.setReg(rd, cpu.getReg(rs1) & imm);
		break;
	case 0x1:
		cpu.setReg(rd, cpu.getReg(rs1) << (imm & 0x1F));
		break;
	case 0x5:
		if ((imm >> 10) & 1)
			cpu.setReg(rd, int32_t(cpu.getReg(rs1)) >> (imm & 0x1F));
		else
			cpu.setReg(rd, cpu.getReg(rs1) >> (imm & 0x1F));
		break;
	}
	return true;
}

/* ===== LOAD ===== */
bool RV32IHandler::handleLoad(CPU &cpu, uint32_t instr, uint32_t &) {
	uint32_t rd = (instr >> 7) & 0x1F;
	uint32_t rs1 = (instr >> 15) & 0x1F;
	int32_t imm = signExtend(instr >> 20, 12);

	uint32_t addr = cpu.getReg(rs1) + imm;
	uint32_t funct3 = (instr >> 12) & 0x7;

	switch (funct3) {
	case 0x0:
		cpu.setReg(rd, signExtend(cpu.getMemory().load8(addr), 8));
		break;
	case 0x1:
		cpu.setReg(rd, signExtend(cpu.getMemory().load16(addr), 16));
		break;
	case 0x2:
		cpu.setReg(rd, cpu.getMemory().load32(addr));
		break;
	case 0x4:
		cpu.setReg(rd, cpu.getMemory().load8(addr));
		break;
	case 0x5:
		cpu.setReg(rd, cpu.getMemory().load16(addr));
		break;
	}
	return true;
}

/* ===== STORE ===== */
bool RV32IHandler::handleStore(CPU &cpu, uint32_t instr, uint32_t &) {
	uint32_t rs1 = (instr >> 15) & 0x1F;
	uint32_t rs2 = (instr >> 20) & 0x1F;
	int32_t imm = signExtend(((instr >> 7) & 0x1F) | ((instr >> 25) << 5), 12);
	uint32_t addr = cpu.getReg(rs1) + imm;

	uint32_t funct3 = (instr >> 12) & 0x7;
	switch (funct3) {
	case 0x0:
		cpu.getMemory().store8(addr, cpu.getReg(rs2) & 0xFF);
		break;
	case 0x1:
		cpu.getMemory().store16(addr, cpu.getReg(rs2) & 0xFFFF);
		break;
	case 0x2:
		cpu.getMemory().store32(addr, cpu.getReg(rs2));
		break;
	}
	return true;
}

/* ===== BRANCH ===== */
bool RV32IHandler::handleBranch(CPU &cpu, uint32_t instr, uint32_t &next_pc) {
	uint32_t rs1 = (instr >> 15) & 0x1F;
	uint32_t rs2 = (instr >> 20) & 0x1F;
	int32_t imm =
	    signExtend(((instr >> 7) & 0x1E) | ((instr >> 20) & 0x7E0) |
			   ((instr << 4) & 0x800) | ((instr >> 19) & 0x1000),
		       13);
	uint32_t funct3 = (instr >> 12) & 0x7;

	switch (funct3) {
	case 0x0:
		if (cpu.getReg(rs1) == cpu.getReg(rs2))
			next_pc = cpu.getPC() + imm;
		break;
	case 0x1:
		if (cpu.getReg(rs1) != cpu.getReg(rs2))
			next_pc = cpu.getPC() + imm;
		break;
	case 0x4:
		if (int32_t(cpu.getReg(rs1)) < int32_t(cpu.getReg(rs2)))
			next_pc = cpu.getPC() + imm;
		break;
	case 0x5:
		if (int32_t(cpu.getReg(rs1)) >= int32_t(cpu.getReg(rs2)))
			next_pc = cpu.getPC() + imm;
		break;
	case 0x6:
		if (cpu.getReg(rs1) < cpu.getReg(rs2))
			next_pc = cpu.getPC() + imm;
		break;
	case 0x7:
		if (cpu.getReg(rs1) >= cpu.getReg(rs2))
			next_pc = cpu.getPC() + imm;
		break;
	}
	return true;
}

/* ===== JAL ===== */
bool RV32IHandler::handleJAL(CPU &cpu, uint32_t instr, uint32_t &next_pc) {
	uint32_t rd = (instr >> 7) & 0x1F;
	int32_t imm =
	    signExtend(((instr >> 21) & 0x3FF) << 1 | ((instr >> 20) & 0x1) << 11 |
			   ((instr >> 12) & 0xFF) << 12 | ((instr >> 31) & 0x1) << 20,
		       21);
	cpu.setReg(rd, cpu.getPC() + 4);
	next_pc = cpu.getPC() + imm;
	return true;
}

/* ===== JALR ===== */
bool RV32IHandler::handleJALR(CPU &cpu, uint32_t instr, uint32_t &next_pc) {
	uint32_t rd = (instr >> 7) & 0x1F;
	uint32_t rs1 = (instr >> 15) & 0x1F;
	int32_t imm = signExtend(instr >> 20, 12);
	cpu.setReg(rd, cpu.getPC() + 4);
	next_pc = (cpu.getReg(rs1) + imm) & ~1;
	return true;
}

/* ===== LUI ===== */
bool RV32IHandler::handleLUI(CPU &cpu, uint32_t instr, uint32_t &) {
	uint32_t rd = (instr >> 7) & 0x1F;
	cpu.setReg(rd, instr & 0xFFFFF000);
	return true;
}

/* ===== AUIPC ===== */
bool RV32IHandler::handleAUIPC(CPU &cpu, uint32_t instr, uint32_t &) {
	uint32_t rd = (instr >> 7) & 0x1F;
	cpu.setReg(rd, cpu.getPC() + (instr & 0xFFFFF000));
	return true;
}

/* ===== SYSTEM ===== */
bool RV32IHandler::handleSystem(CPU &cpu, uint32_t instr, uint32_t &) {
	cpu.setRunning(false);
	return true;
}
