#include "cpu.h"

#include <cassert>
#include <cstdint>
#include <iostream>

// Used to implement mirroring in CPU ram. CPU ram addresses are mirrored every 2048 bytes (0x0800)
const uint16_t CPU_RAM_ADDRESS_MASK{0x07FF};
// The max address for the CPU RAM addressable range
const uint16_t CPU_RAM_MAX_RANGE{0x1FFF};

const char NEWLINE{'\n'};
const int BITS_PER_BYTE{8};

namespace nestalgic {

auto CPUBus::Read(uint16_t address) -> uint8_t {
  // RAM region: $0000 - $1FFF (mirrored every $0800), address & 0x07FF isolates only the lower 11 bits
  if (address >= 0x0000 && address <= CPU_RAM_MAX_RANGE) {
    return ram_.at(address & CPU_RAM_ADDRESS_MASK);
  }

  // Log error message and return a safe default value
  std::cerr << "Error: Invalid address provided Address should be between 0x0000 - 0xFFFF."
            << NEWLINE;
  return 0;
}

auto CPUBus::Write(uint16_t address, uint8_t data) -> void {
  // RAM region: $0000 - $1FFF (mirrored every $0800), address & 0x07FF isolates only the lower 11 bits
  if (address >= 0x0000 && address <= CPU_RAM_MAX_RANGE) {
    ram_.at(address & CPU_RAM_ADDRESS_MASK) = data;
  }

  // Log error message and return a safe default value
  std::cerr << "Error: Invalid address provided Address should be between 0x0000 - 0xFFFF."
            << NEWLINE;
}
}  // namespace nestalgic
