#ifndef NESTALGIC_CPU_H_
#define NESTALGIC_CPU_H_

#include <array>
#include <cstdint>

#include "bus.h"

const int CPU_RAM_SIZE{0x0800};  // 2048 (2KB of ram)

namespace nestalgic {

// Represents the NES CPU bus, handling memory reads and writes.
//
// The CPU bus determines which memory-mapped resource (RAM, PPU, APU, or cartridge ROM)
// should handle a given address based on the NES memory map.
//
// NES Memory Map: https://www.nesdev.org/wiki/CPU_memory_map
//
// ┌──────────────────┬────────┬──────────────────────────────────────────────┐
// │ Address Range    │  Size  │ Description                                  │
// ├──────────────────┼────────┼──────────────────────────────────────────────┤
// │ $0000 - $07FF    │ 2 KB   │ Internal RAM                                 │
// │ $0800 - $1FFF    │ Mirror │ Repeats every $0800 bytes                    │
// │ $2000 - $2007    │ 8 B    │ PPU Registers                                │
// │ $2008 - $3FFF    │ Mirror │ PPU Regs (Mirrored every 8 bytes)            │
// │ $4000 - $4017    │ 24 B   │ APU and I/O Registers                        │
// │ $4018 - $401F    │ 8 B    │ APU and I/O (typically unused)               │
// │ $4020 - $FFFF    │ 48 KB  │ Cartridge ROM, PRG RAM, and Mapper Registers │
// └──────────────────┴────────┴──────────────────────────────────────────────┘
class CPUBus : public Bus {
 public:
  // Reads a byte from memory at the given address.
  //
  // Args:
  //   address: The 16-bit memory address to read from.
  //
  // Returns:
  //   The byte value stored at the given memory address.
  auto Read(uint16_t address) -> uint8_t override;

  // Writes a byte to memory at the given address.
  //
  // Args:
  //   address: The 16-bit memory address to write to.
  //   data: The 8-bit data to write to the memory address.
  //
  // Returns:
  //   The byte value stored at the given memory address.
  auto Write(uint16_t address, uint8_t data) -> void override;

 private:
  // 2KB of internal RAM on the CPU bus
  std::array<uint8_t, CPU_RAM_SIZE> ram_{};
};
}  // namespace nestalgic
#endif  // NESTALGIC_CPU_H_
