/* memory.hpp */

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>
#include <cstddef>
#include <cstdint>

class Memory {
      private:
	static constexpr size_t SIZE = 1024 * 1024;
	std::array<uint8_t, SIZE> data;

      public:
	Memory();

	/* ===== LOAD ===== */
	uint8_t load8(uint32_t addr) const;
	uint16_t load16(uint32_t addr) const;
	uint32_t load32(uint32_t addr) const;

	/* ===== STORE ===== */
	void store8(uint32_t addr, uint8_t value);
	void store16(uint32_t addr, uint16_t value);
	void store32(uint32_t addr, uint32_t value);

	/* ===== BULK ===== */
	void write(uint32_t addr, const uint8_t *src, size_t size);
	void zero(uint32_t addr, size_t size);

	size_t getSize() const;
};

#endif