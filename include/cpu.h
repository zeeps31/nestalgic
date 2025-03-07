#ifndef NESTALGIC_CPU_H_
#define NESTALGIC_CPU_H_

#include <array>
#include <bitset>
#include <cstdint>
#include <unordered_map>

#include "bus.h"

const int CPU_RAM_SIZE{0x0800};  // 2048 (2KB of ram)

namespace nestalgic {

// Forward declare the class before using it in CPU
namespace internal {
class CPUTestHelper;
}

enum class AddressingMode : std::uint8_t { Immediate };
enum CPUStatusFlag : std::uint8_t {
  Negative = 7,
  Overflow = 6,
  Decimal = 3,
  InterruptDisable = 2,
  Zero = 1,
  Carry = 0
};

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

// A high-level, non-cycle-accurate implementation of the MOS 6502 processor.
//
// This class emulates the 8-bit Ricoh 2A03 CPU (based on the MOS 6502) used in the NES.
// It handles instruction decoding, memory access, and register management, but does
// not implement cycle-accurate behavior.
//
// This implementation follows the general behavior of the NES CPU, but timing-sensitive
// operations (such as cycle-exact sprite evaluation and interrupts) may not be accurate.
//
// More details: https://www.nesdev.org/wiki/CPU
class CPU {
 public:
  // Constructs a CPU instance and connects it to the given bus.
  //
  // Args:
  //   cpuBus: Pointer to the system bus, used for memory reads/writes.
  explicit CPU(Bus* cpuBus);

  // Executes a single CPU cycle.
  //
  // The CPU fetches, decodes, and executes one instruction step per call.
  void Clock();

  // Resets the CPU to its initial state.
  //
  // This resets all registers, clears internal state, and sets the program counter
  // to the address stored in the reset vector ($FFFC-$FFFD).
  void Reset();

 private:
  class CPUStatus {
    // NOLINTBEGIN(*-magic-numbers) 8 is used as this will represent a single byte (8 bits)
    std::bitset<8> flags;
    // NOLINTEND(*-magic-numbers)

   public:
    void Set(CPUStatusFlag flag, bool value) { flags.set(flag, value); }

    auto Get(CPUStatusFlag flag) -> bool { return flags.test(flag); }

    void FromByte(uint8_t value) {
      // NOLINTBEGIN(*-magic-numbers) 8 is used as this will represent a single byte (8 bits)
      flags = std::bitset<8>(value);
      // NOLINTEND(*-magic-numbers)
    }

    auto ToByte() -> uint8_t { return static_cast<uint8_t>(flags.to_ulong()); }
  };

  // Internal registers and bus
  Bus* cpu_bus_;

  uint8_t a_{0x00};
  uint8_t x_{0x00};
  uint8_t y_{0x00};
  uint16_t pc_{0x0000};
  uint8_t s_{0xFF}; // NOLINT(*-magic-numbers)
  CPUStatus p_;

 private:
  // Internal helper functions and class members
  friend class internal::CPUTestHelper;

  uint8_t opcode_ = 0x00;
  uint8_t operand_ = 0x00;  // The value retrieved using the addressing mode.
  int remaining_cycles_ = 0;

  auto Read(uint16_t address) -> uint8_t;
  auto FetchOperand(AddressingMode addressingMode) -> uint8_t;

 private:
  // Opcode declarations
  struct OpcodeEntry {
    AddressingMode addressingMode;
    void (CPU::*operation)();  // Points to a member function of the class (opcode)
    int cycleCount;
  };

  std::unordered_map<uint8_t, OpcodeEntry> operations_;
  void LoadOperations();
  void LDA();
};

namespace internal {
// A helper class for unit testing the CPU.
//
// This class provides direct access to the CPU's internal registers for testing purposes.
// It allows test cases to manipulate and verify the CPU state without executing instructions.
//
// This class should only be used in test environments and is not part of the public API.
class CPUTestHelper {
 public:
  // Sets the CPU's accumulator register.
  //
  // Args:
  //   cpu: Reference to the CPU instance.
  //   accumulatorValue: The value to set in the accumulator (A register).
  static void SetAccumulator(CPU& cpu, uint8_t accumulatorValue);

  // Retrieves the value of the CPU's accumulator register.
  //
  // Args:
  //   cpu: Reference to the CPU instance.
  //
  // Returns:
  //   The current value of the accumulator.
  static auto GetAccumulator(CPU& cpu) -> uint8_t;

  // Sets the CPU's program counter (PC).
  //
  // Args:
  //   cpu: Reference to the CPU instance.
  //   programCounter: The new program counter value.
  static void SetProgramCounter(CPU& cpu, uint16_t programCounter);

  // Retrieves the CPU's current program counter value.
  //
  // Args:
  //   cpu: Reference to the CPU instance.
  //
  // Returns:
  //   The current program counter value.
  static auto GetProgramCounter(CPU& cpu) -> uint16_t;

  // Retrieves a status flag from the CPU
  //
  // Args:
  //   cpu: Reference to the CPU instance.
  //   flag: The CPUStatusFlag to retrieve
  //
  // Returns:
  //   The value at the status flag
  static auto GetStatusFlag(CPU& cpu, CPUStatusFlag flag) -> bool;
};
};  // namespace internal
}  // namespace nestalgic
#endif  // NESTALGIC_CPU_H_
