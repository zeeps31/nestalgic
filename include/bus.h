#ifndef NESTALGIC_BUS_H_
#define NESTALGIC_BUS_H_

#include <cstdint>

namespace nestalgic {
// Interface to represent a "Bus" on the NES.
//
// Both the CPU and PPU have busses that are connected to various components on the
// NES.
class Bus {
 public:
  Bus() = default;
  // Ensure proper cleanup in derived classes
  virtual ~Bus() = default;

  // There should not be any scenarios where a Bus will be copied
  Bus(const Bus& bus) = delete;
  auto operator=(const Bus&) -> Bus& = delete;

  // Prevent moving. Bus should only be initialized once and does not need to be be transferred
  Bus(Bus&&) = delete;
  auto operator=(const Bus&&) -> Bus& = delete;

  virtual auto Read(uint16_t address) -> uint8_t = 0;
  virtual void Write(uint16_t address, uint8_t data) = 0;
};
}  // namespace nestalgic
#endif  // NESTALGIC_BUS_H_
