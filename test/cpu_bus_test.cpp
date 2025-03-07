#include <gtest/gtest.h>

#include <cstdint>
#include <tuple>

#include "cpu.h"

// A test suite for verifying CPUBus memory read/write behavior.
//
// This parameterized test ensures that writing data to a specific memory address
// and reading it from another address produces the expected result. This is
// particularly useful for testing memory mirroring, where certain addresses map
// to the same underlying memory region.
namespace {
class CPUBusTest : public ::testing::TestWithParam<std::tuple<uint16_t, uint16_t, uint8_t>> {
 protected:
  nestalgic::CPUBus testObject;  // Object under test
};

// Tests CPUBus RAM access by writing a byte to a memory address and
// reading it from a potentially mirrored address.
//
// This test is designed to verify both standard memory reads and
// memory mirroring behavior when implemented.
//
// Args:
//   writeAddress: The memory address where data is written.
//   readAddress: The memory address from which data is read.
//   data: The byte value to write and expect when reading.
TEST_P(CPUBusTest, TestRamAccess) {
  // Arrange
  const uint16_t writeAddress = std::get<0>(GetParam());
  const uint16_t readAddress = std::get<1>(GetParam());
  const uint8_t data = std::get<2>(GetParam());

  // Act
  testObject.Write(writeAddress, data);

  // Assert
  EXPECT_EQ(testObject.Read(readAddress), data);
}

// Instantiates test cases for CPUBus memory access.
//
// This set of test cases ensures that writing to one memory location and
// reading from another (potentially mirrored) location behaves correctly.
//
// Test Cases:
//   (0x0000, 0x0000, 0x01) - Writing to address 0x0000 and reading from 0x0000.
//   (0x0000, 0x0800, 0x01) - Writing to address 0x00800 and reading from 0x0000. The address should
//   be mirrored back to 0x0000.
//   (0x1FFF, 0x07FF, 0x01) - Writing to address 0x1FFF (the maximum of RAM addressable range)
//   should mirror back to 0x07FF (the maximum capacity of the RAM)
INSTANTIATE_TEST_SUITE_P(CPURamAccessTestCases, CPUBusTest,
                         ::testing::Values(std::make_tuple(0x0000, 0x0000, 0x01),
                                           std::make_tuple(0x0800, 0x0000, 0x01),
                                           std::make_tuple(0x1FFF, 0x07FF, 0x01)));

}  // namespace
