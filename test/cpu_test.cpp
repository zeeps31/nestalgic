#include <cstdint>
#include <tuple>
#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include "bus.h"
#include "cpu.h"

using ::testing::Return;
using nestalgic::internal::CPUTestHelper;

namespace {

class MockCPUBus : public nestalgic::Bus {
 public:
  // Trailing return types are not supported in MOCK_METHOD
  MOCK_METHOD(uint8_t, Read, (uint16_t address), (override)); // NOLINT (*-use-trailing-return-type)
  MOCK_METHOD(void, Write, (uint16_t address, uint8_t data), (override));
};

// Parameterized test for CPU transfer operations.
//
// This test verifies the behavior of CPU register transfers using different
// combinations of input values. Each test case is defined as a tuple.
//
// Args:
//   - uint8_t: Initial value read at the program counter. This is the opcode.
//   - uint8_t: The value of the operand when the read from the address at the PC.
//   - int: Expected cycle count for the operation.
//   - bool: Whether the Negative flag (N) should be set after execution.
//   - bool: Whether the Zero flag (Z) should be set after execution.
class CPUTransferOperationsTest
    : public ::testing::TestWithParam<std::tuple<uint8_t, uint8_t, int, bool, bool>> {
 protected:
  MockCPUBus mockCPUBus;
  nestalgic::CPU testObject{&mockCPUBus};  // Object under test
};

// Tests the LDA (Load Accumulator) operation using Immediate Addressing Mode.
//
// This parameterized test verifies that the CPU correctly loads a value into the accumulator
// from an immediate operand, sets the appropriate status flags, and consumes the correct
// number of cycles.
TEST_P(CPUTransferOperationsTest, TestLDAOperationImmediateAddressing) {
  // Set the CPU program counter to the start of execution.
  CPUTestHelper::SetProgramCounter(testObject, 0x0000);

  // Expect the CPU to fetch the opcode (LDA immediate).
  EXPECT_CALL(mockCPUBus, Read(0x0000)).WillOnce(Return(std::get<0>(GetParam())));  // Return opcode

  // Expect the CPU to fetch the operand (immediate value).
  EXPECT_CALL(mockCPUBus, Read(0x0001))
      .WillOnce(Return(std::get<1>(GetParam())));  // Return operand

  // Clock the CPU for the number of cycles required by the operation.
  for (int i = 0; i < std::get<2>(GetParam()); i++) {
    testObject.Clock();
  }

  // Clock the CPU for the number of cycles required by the operation.
  EXPECT_EQ(CPUTestHelper::GetAccumulator(testObject), std::get<1>(GetParam()));

  // Verify that the Negative flag (N) is set correctly.
  EXPECT_EQ(CPUTestHelper::GetStatusFlag(testObject, nestalgic::CPUStatusFlag::Negative),
            std::get<3>(GetParam()));

  // Verify that the Zero flag (Z) is set correctly.
  EXPECT_EQ(CPUTestHelper::GetStatusFlag(testObject, nestalgic::CPUStatusFlag::Zero),
            std::get<4>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(TestLDAOperationValues, CPUTransferOperationsTest,
                         ::testing::Values(std::make_tuple(0xA9, 0x01, 2, 0, 0),
                                           std::make_tuple(0xA9, 0x00, 2, 0, 1),
                                           std::make_tuple(0xA9, 0x80, 2, 1, 0)));
}  // namespace
