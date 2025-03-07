#include "cpu.h"

#include <cassert>
#include <cstdint>
#include <iostream>

#include "bus.h"

// The low byte of where the reset vector address. This value and the value stored at 0xFFFD are
// combined to form a single address that is used to set the program counter upon reset
const uint16_t RESET_VECTOR_LOW_BYTE{0xFFFC};

// Used to implement mirroring in CPU ram. CPU ram addresses are mirrored every 2048 bytes (0x0800)
const uint16_t CPU_RAM_ADDRESS_MASK{0x07FF};
// The max address for the CPU RAM addressable range
const uint16_t CPU_RAM_MAX_RANGE{0x1FFF};

const uint8_t STACK_POINTER_DEFAULT{0xFF};

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

// Connects the CPUBus to the CPU and loads operations into an opcode mapping.
CPU::CPU(Bus* cpuBus) : cpu_bus_(cpuBus) {
  assert(cpu_bus_ != nullptr);
  LoadOperations();
}

// Each time the CPU is clocked and the remaining cycle counter is 0, the
// following events occur:
//
// 1. The CPU reads from CPUBus at the address stored in the program counter.
// 2. The program counter is incremented by one.
// 3. The operation retrieved from the address at the program counter is executed.
//
// If there are still remaining cycles for the operation, the remaining
// cycle counter is decremented and no action is performed.
void CPU::Clock() {
  if (remaining_cycles_ == 0) {
    opcode_ = Read(pc_++);
    const OpcodeEntry opcodeEntry = operations_[opcode_];

    operand_ = FetchOperand(opcodeEntry.addressingMode);
    (this->*opcodeEntry.operation)();
    remaining_cycles_ += opcodeEntry.cycleCount;
  }

  remaining_cycles_--;
}

void CPU::Reset() {
  // Reset registers to their initial state
  a_ = 0x00;
  x_ = 0x00;
  y_ = 0x00;
  s_ = STACK_POINTER_DEFAULT;
  p_.FromByte(0x00);

  // Fetch the reset vector ($FFFC-$FFFD), which contains the program's entry point
  const uint8_t pc_lo{Read(RESET_VECTOR_LOW_BYTE)};
  const uint8_t pc_hi{Read(RESET_VECTOR_LOW_BYTE + 1)};

  // Combine the low and high bytes to form the 16-bit program counter (little-endian format)
  pc_ = pc_hi << BITS_PER_BYTE | pc_lo;

  // As the reset line goes high the processor performs a start sequence of 7 cycles
  const int reset_cycle_count = 7;
  remaining_cycles_ += reset_cycle_count;
}

// Read a value from the CPU bus. This method is a wrapper around the CPU bus. All reads will simply
// be delegated to the CPUBus.
auto CPU::Read(uint16_t address) -> uint8_t { return cpu_bus_->Read(address); }

// Fetches the operand for the current operation based on the addressing mode
//
// Uses the AddressingMode enum to determine how the operand should be fetched, executes the
// addressing mode and returns the value.
auto CPU::FetchOperand(AddressingMode addressingMode) -> uint8_t {
  switch (addressingMode) {
    case AddressingMode::Immediate:
      return Read(pc_++);
  }

  // Log an error message and return a safe default value
  std::cerr << "Error: Unsupported addressing mode in FetchOperand()" << NEWLINE;
  return 0;  // Default return value
}

// Load Accumulator with Memory (LDA)
void CPU::LDA() {
  a_ = operand_;

  p_.Set(CPUStatusFlag::Negative, (a_ & 0x80) != 0x00); // NOLINT(*-magic-numbers)
  p_.Set(CPUStatusFlag::Zero, a_ == 0x00);
}

void CPU::LoadOperations() {
  // NOLINTBEGIN(*-magic-numbers)
  operations_ = {{0xA9, {AddressingMode::Immediate, &CPU::LDA, 2}}};
  // NOLINTEND(*-magic-numbers)
}

namespace internal {
void CPUTestHelper::SetAccumulator(CPU& cpu, uint8_t accumulatorValue) {
  cpu.a_ = accumulatorValue;
}

auto CPUTestHelper::GetAccumulator(CPU& cpu) -> uint8_t { return cpu.a_; }

void CPUTestHelper::SetProgramCounter(CPU& cpu, uint16_t programCounter) {
  cpu.pc_ = programCounter;
}

auto CPUTestHelper::GetProgramCounter(CPU& cpu) -> uint16_t { return cpu.pc_; }

auto CPUTestHelper::GetStatusFlag(CPU& cpu, CPUStatusFlag flag) -> bool { return cpu.p_.Get(flag); }
}  // namespace internal
}  // namespace nestalgic
