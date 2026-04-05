/* memory.cpp */

#include "../include/memory.hpp"
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

static void checkBounds(uint32_t addr, size_t width, size_t limit) {
	if (addr + width > limit) {
		std::ostringstream oss;
		oss << "Memory access out of bounds at 0x"
		    << std::hex << std::uppercase << addr;
		throw std::out_of_range(oss.str());
	}
}

static void checkAlign(uint32_t addr, size_t align) {
	if (addr % align != 0) {
		std::ostringstream oss;
		oss << "Misaligned memory access at 0x"
		    << std::hex << std::uppercase << addr;
		throw std::runtime_error(oss.str());
	}
}

Memory::Memory() { data.fill(0); }

size_t Memory::getSize() const { return SIZE; }

/* ===== LOAD ===== */
uint8_t Memory::load8(uint32_t addr) const {
	checkBounds(addr, 1, SIZE);
	return data[addr];
}

uint16_t Memory::load16(uint32_t addr) const {
	checkAlign(addr, 2);
	checkBounds(addr, 2, SIZE);
	return static_cast<uint16_t>(data[addr]) |
	       static_cast<uint16_t>(data[addr + 1]) << 8;
}

uint32_t Memory::load32(uint32_t addr) const {
	checkAlign(addr, 4);
	checkBounds(addr, 4, SIZE);
	return static_cast<uint32_t>(data[addr]) |
	       static_cast<uint32_t>(data[addr + 1]) << 8 |
	       static_cast<uint32_t>(data[addr + 2]) << 16 |
	       static_cast<uint32_t>(data[addr + 3]) << 24;
}

/* ===== STORE ===== */
void Memory::store8(uint32_t addr, uint8_t value) {
	checkBounds(addr, 1, SIZE);
	data[addr] = value;
}

void Memory::store16(uint32_t addr, uint16_t value) {
	checkAlign(addr, 2);
	checkBounds(addr, 2, SIZE);
	data[addr] = value & 0xFF;
	data[addr + 1] = (value >> 8) & 0xFF;
}

void Memory::store32(uint32_t addr, uint32_t value) {
	checkAlign(addr, 4);
	checkBounds(addr, 4, SIZE);
	data[addr] = value & 0xFF;
	data[addr + 1] = (value >> 8) & 0xFF;
	data[addr + 2] = (value >> 16) & 0xFF;
	data[addr + 3] = (value >> 24) & 0xFF;
}

/* ===== BULK ===== */
void Memory::write(uint32_t addr, const uint8_t *src, size_t size) {
	checkBounds(addr, size, SIZE);
	std::memcpy(&data[addr], src, size);
}

void Memory::zero(uint32_t addr, size_t size) {
	checkBounds(addr, size, SIZE);
	std::memset(&data[addr], 0, size);
}