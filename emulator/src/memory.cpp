/* memory.cpp */

#include "../include/memory.hpp"
#include <cstring>

Memory::Memory() { data.fill(0); }

size_t Memory::getSize() const { return SIZE; }

/* ===== LOAD ===== */
uint8_t Memory::load8(uint32_t addr) const { return data[addr]; }

uint16_t Memory::load16(uint32_t addr) const {
	return data[addr] | (data[addr + 1] << 8);
}

uint32_t Memory::load32(uint32_t addr) const {
	return data[addr] | (data[addr + 1] << 8) | (data[addr + 2] << 16) |
	       (data[addr + 3] << 24);
}

/* ===== STORE ===== */
void Memory::store8(uint32_t addr, uint8_t value) { data[addr] = value; }

void Memory::store16(uint32_t addr, uint16_t value) {
	data[addr] = value & 0xFF;
	data[addr + 1] = (value >> 8) & 0xFF;
}

void Memory::store32(uint32_t addr, uint32_t value) {
	data[addr] = value & 0xFF;
	data[addr + 1] = (value >> 8) & 0xFF;
	data[addr + 2] = (value >> 16) & 0xFF;
	data[addr + 3] = (value >> 24) & 0xFF;
}

/* ===== BULK ===== */
void Memory::write(uint32_t addr, const uint8_t *src, size_t size) {
	std::memcpy(&data[addr], src, size);
}

void Memory::zero(uint32_t addr, size_t size) {
	std::memset(&data[addr], 0, size);
}